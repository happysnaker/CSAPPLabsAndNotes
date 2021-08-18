## 实验之前
这个实验难度比较适中，当然前提是你第八章认真研究过了几遍，在做这个实验之前，请必须阅读以便官网的writeup文档，否则你可能不明白这个实验要实现干点什么，然后下载官网的实验材料到你的电脑上：
Write文档：
[Writeup帮助文档链接](http://csapp.cs.cmu.edu/3e/shlab.pdf)


实验材料：
[实验材料下载链接](http://csapp.cs.cmu.edu/3e/shlab-handout.tar)

**实验说明**
1. 你只能修改tsh.c文件来完成其中的7个函数：
- eval：解析和解释命令行的主例程。 [大约70行]
- biltin_cmd：识别并解释内置命令：quit，fg，bg和job。 [大约25行]
- do_bgfg：实现bg和fg内置命令。 [大约50行]
- waitfg：等待前台作业完成。 [大约20行]
- sigchld处理程序：处理SIGCHILD信号。 [大约80行]
- sigint处理程序：处理SIGINT（ctrl-c）信号。 [大约15行]
- sigtstp处理程序：处理SIGTSTP（ctrl-z）信号。 [大约15行]

2.  每次修改tsh.c文件后，你都需要键入单独的一个```make```命令来完成一系列准备工作，官方提供了16个测试来检验你的答案，你需要键入```make testXX```来输出自己的答案，然后输入```make rtestXX```来比对标准答案，其中XX = 01、02、03、......、16。你的答案必须和标准答案一样才算成功(进程号可以是不同的，因为它们是随机的)，例如：
```
make test03
这里会输出你的答案

make rtest03
这里是标准答案

```

3. 在tsh.c文件中，老师已经帮我们实现了很多辅助函数，在代码中我会标识它的作用(读一读是很有帮助的)，并且已经定义了一些全局变量，大大降低了实验难度，我们需要充分理解这些变量和函数。如下，这些是已经准备好的函数或者变量：
```
extern char **environ;      /* defined in libc 这个时环境变量，exec的参数，老师已经安排好了*/
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
/*进程结构体，保管着所有tsh所有子进程的信息，必要的时候，我们必须要修改它*/
struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */

int parseline(const char *cmdline, char **argv); 		/*解析命令行的函数，和书上一样*/ 
void sigquit_handler(int sig);							/*quit信号处理*/
void clearjob(struct job_t *job);						/*清理进程链表，退出的时候用*/
void initjobs(struct job_t *jobs);						/*初始化进程链表*/
int maxjid(struct job_t *jobs); 						/*找到最大的进程PID*/
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);	/*添加进程，这个是需要我们手动添加的*/
int deletejob(struct job_t *jobs, pid_t pid); 			/*删除进程，依然需要我们手动删除*/
pid_t fgpid(struct job_t *jobs);						/*如果有前台工作进程，返回1，否则返回0*/
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);	/*通过pid获得对于的进程结构体指针*/ 
struct job_t *getjobjid(struct job_t *jobs, int jid); 	/*通过jid获得对于的进程结构体指针*/
int pid2jid(pid_t pid); 								/*返回对于pid进程的jid*/
void listjobs(struct job_t *jobs);						/*打印进程信息*/
void usage(void);										/*不用管他*/
void unix_error(char *msg);								/*打印错误信息*/
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
```

4. 用户键入的命令行应由一个名称和零个或多个参数组成，所有参数以一个或多个空格分隔。如果name是内置命令，则tsh应该立即处理它并等待下一个命令行。否则，tsh应该假定名称是可执行文件，它会在初始子进程的上下文中加载并运行（在这种情况下，工作一词是指此初始子流程。有几个需要注意的地方：
- tsh不需要支持管道（|）或I / O重定向（<和>）。
- 键入ctrl-c（ctrl-z）应该会导致SIGINT（SIGTSTP）信号发送到当前的前地面作业以及该作业的任何后代（例如，它派生的任何子进程）。如果没有前台作业，则该信号应该没有任何作用。
- 如果命令行以＆结束，则tsh应该在后台运行作业。否则，它应该在前台运行作业。
- 每个作业都可以由进程ID（PID）或作业ID（JID）标识，该ID是一个正整数由tsh分配。 JID应该在命令行上以前缀“％”表示。例如，“％5” 表示JID 5，“5”表示PID5。（我们已为您提供了所需的所有例程处理工作清单。）
- tsh应该支持以下内置命令：
– quit命令终止tsh程序。
– jobs命令列出所有后台作业。
– bg <job>命令通过向其发送SIGCONT信号来重新启动<job>，然后在背景。 <job>参数可以是PID或JID。
– fg <job>命令通过向其发送SIGCONT信号来重新启动<job>，然后在前景。 <job>参数可以是PID或JID。

5. 正如前面所说的，我们的答案必须要和标准答案相同，所以在特定的位置我们需要输出特定的语句。例如，在后台工作运行时，我们要打印它的一系列参数等等等。


## 实验代码 
### Fork函数
这个不是要求的，但我们仍然包装好它：
```
/*自定义安全的Fork*/
pid_t Fork(void){
	pid_t pid;
	if ((pid = fork()) < 0)
		unix_error("fork error!");
	return pid;
}
```

----------------------------------
---------------------------------
### eval函数
这个函数的重点就是要小心同步并发流中的竞争，此外，setpgid也是很有必要的，它将子进程组与tsh进程组分开，避免tsh收到莫名的信号而停止。
```
/*利用书上已有的框架和543提到的避免并发竞争发生*/
void eval(char *cmdline) 
{
	char* argv[MAXARGS];
	char* buf[MAXLINE];
	int bg;
	pid_t pid;
	sigset_t mask_all, mask_one, prev_one;
	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return;
	
	sigfillset(&mask_all);
	sigemptyset(&mask_one);
	sigaddset(&mask_one, SIGCHLD);

	if (!builtin_cmd(argv)){
		sigprocmask(SIG_BLOCK, &mask_one, &prev_one);/*必须要锁住，防止addjob和由子进程终止引起的信号处理竞争*/
		if ((pid = Fork()) == 0){
			/*tsh派生子进程默认和子进程在一个进程组下*/
			/*setpgid将子进程组和tsh进程组分开来，避免停止子进程组把tsh一起停止掉，同时子进程组id就等于pid，发送消息很方便*/
			/*请注意进程id并不等于题目中的jid，但jid和pid都可以用来标识一个作业*/
			if (setpgid(0, 0) < 0){
				perror("setpgid error!");
				exit(0);
			}
			sigprocmask(SIG_SETMASK, &prev_one, NULL);/*子进程中不需要堵住它，但父进程需要*/
			if (execve(argv[0], argv, environ) < 0){
				printf ("%s: Command not found\n", argv[0]);
				exit(0);
			}
		}
		if (!bg){
			sigprocmask(SIG_BLOCK, &mask_all, NULL);/*锁住一切信号，避免addjob函数被中断*/
			addjob(jobs, pid, FG, cmdline);
			sigprocmask(SIG_SETMASK, &prev_one, NULL);
			waitfg(pid);
		}
		else{
			sigprocmask(SIG_BLOCK, &mask_all, NULL);
			addjob(jobs, pid, BG, cmdline);
			printf ("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
			sigprocmask(SIG_SETMASK, &prev_one, NULL);/*打印全局变量，仍然需要加塞，防止途中被中断，可能造成还未读(写)内存而内存的值却被修改的情况*/
		}
	}
	//printf ("eve return\n");
    return;

```
-------------------------------
-------------------------------

### builtin_cmd函数
这个函数我们按要求解析4个内置命令就好了
```
int builtin_cmd(char **argv) 
{
	//printf ("cmd\n");
	if (strcmp(argv[0], "quit") == 0)
		exit(0);	
    	else if (strcmp(argv[0], "&") == 0)
		return 1;
	else if (strcmp(argv[0], "fg") == 0)
		do_bgfg(argv);
	else if (strcmp(argv[0], "bg") == 0)
		do_bgfg(argv);
	else if (strcmp(argv[0], "jobs") == 0)
		listjobs(jobs); /*这个是老师写好的辅助函数，实现起来也很简单*/
	else
		//printf ("cmd before return :%s \n",argv[0]);
		return 0;/*如果不是内置命令，返回1*/
	return 1;     /* not a builtin command */
}
```
--------------------------
--------------------------
###do_bgfg函数
这个函数发生继续信号到指定进程中，并指定是以fg模式还是bg模式运行，这意味着原来的bg进程可能会变成fg进程，所以要注意修改信息。
```
void do_bgfg(char **argv) 
{
	//printf ("bgfg\n");
	struct job_t* job = NULL;
	if (argv[1] == NULL){
		printf ("%s command requires PID or %jobid argument\n", argv[0]);
		return;
	}
	int idex;
	/*解析pid*/
	if (sscanf(argv[1], "%d", &idex) > 0){
		if ((job = getjobpid(jobs, idex)) == NULL){
			printf ("%s: No such process\n", argv[1]);
			return;
		}
	}
	/*解析jid*/
	else if (sscanf(argv[1], "%%%d", &idex) > 0){
		if ((job = getjobjid(jobs, idex)) == NULL){
			printf ("%s: No such process\n", argv[1]);
			return;
		}	
	}
	/*都失败的话，打印错误消息*/
	else{
		printf ("%s: argument must be a PID or %%jobid\n", argv[0]);
		return;
	}

	/*发送信号，这里要求发送到进程组，所以采用负数*/
	/*之前setpid后，子进程的进程组id和pid是一致的，请不要将jid和pid搞混了*/
	kill(-(job->pid), SIGCONT);
	if (strcmp(argv[0], "bg") == 0){
		job->state = BG;/*设置状态*/
		printf ("[%d] (%d) %s", job->jid, job->pid, job->cmdline);		
	}
	else{
		job->state = FG;/*设置状态*/
		waitfg(job->pid);
	}
	//printf ("bgfg return");
    return;
}
```
------------
------------
###waitfg函数
等待前台任务结束，可以利用老师给定函数判定，注意这里不能使用简单的pause，因为信号可能会在执行pause前到来(恰好)，这样就会形成竞争关系，如果程序这个时候执行了pause的话，就会等待着一个永远不会到来的信号(可能永远不会醒过来)。所以要使用sigsuspend函数，它等价于三条语句(请翻书)，第一条语句与pause是原子属性的，它是不可中断的，在此之前我们堵塞SIGCHLD信号，然后在执行sigsuspend第一条语句时，我们短暂的解除堵塞，然后原子的立即执行pause，如果在执行sigsuspend之前SIGCHLD信号发送过来，它会被堵塞，而当sigsuspend执行时，它会等在pause执行时被释放，这会唤醒pause，同时陷入处理程序，设置while循环条件，结束循环，如果没有信号到来，那么pause就会正确执行。
当然这里也可以简单的用sleep语句，但是书上545页也陈述了它的缺点，最好还是利用sigsuspend指令。
```
void waitfg(pid_t pid)
{
	sigset_t mask, prev;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, &prev);
	while (fgpid(jobs) != 0){
		sigsuspend(&mask);
		//printf ("wait here\n");
	}
	sigprocmask(SIG_SETMASK, &prev, NULL);
    return;
}
```
--------------
-------------

###sigchld_handler函数
这个函数要处理三种情况，一是处理正常中止的情况，二是收到信号中止的情况，三是被信号所暂时停止的情况，前两种情况都要显示的将进程从进程表中删除，第三种情况却不用，但是却要更改状态。为了防止del程序被中断，我们仍然需要加塞。
```
void sigchld_handler(int sig) 
{
	int olderrno = errno;
	int status;
	pid_t pid;
	struct job_t *job;
	sigset_t mask, prev;
	sigfillset(&mask);
	while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0){
		sigprocmask(SIG_BLOCK, &mask, &prev);
		if (WIFEXITED(status)){
			deletejob(jobs, pid);
		}
		else if (WIFSIGNALED(status)){
			printf ("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
			deletejob(jobs, pid);
		}
		else if (WIFSTOPPED(status)){
			printf ("Job [%d] (%d) stoped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
			job = getjobpid(jobs,pid);
			job->state = ST;
		}
		sigprocmask(SIG_SETMASK, &prev, NULL);			
	}
	errno = olderrno;
	//printf ("chldreturn\n");
    return;
}
```
-----------------
----------------------
###sigint_handler函数和sigtstp_handler函数
最后两个函数比较简单，我们只要负责将信号发送给前台作业即可。因为这两个信号都是有默认行为的，不需要我们瞎操心。
```
void sigint_handler(int sig) 
{
	pid_t pid;
	if ((pid = fgpid(jobs)) > 0)
		kill(-pid, sig);
    return;
}
```
```
void sigtstp_handler(int sig) 
{
	pid_t pid;
	if ((pid = fgpid(jobs)) > 0)
		kill(-pid, sig);
    return;
}
```




-----
-----
----------
------------
##总结
虽然只需要我们实现7个函数，但是阅读老师给定帮助函数是非常有帮助的。做这个实验可能更多的要处理好并发造成的问题吧，并发编程是非常容易出错的，我们必须小心小心再小心。
完整的tsh.c文件下载：
[https://github.com/happysnaker/CSAPPLabs/blob/CSAPP/tsh.c](https://github.com/happysnaker/CSAPPLabs/blob/CSAPP/tsh.c)
