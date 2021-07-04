### 实验准备
请前往[CSAPP官网]((http://csapp.cs.cmu.edu/3e/labs.html
)下载实习手册并仔细阅读Writeup文档，这很重要。
[Writeup中文文档(机翻)](https://github.com/happysnaker/CSAPPLabs/blob/CSAPP/proxy.c%20Writeup.txt)

#### 实验说明
这个实验需要我们实现一个代理服务器，接受来自客户端的请求并发送给服务器，然后接受来自服务器的回答并转发给客户端。

我们需要完成三大主要的功能：
1. 实现基本的转发功能。
2. 实现并发功能。
3. 实现带近似LRU机制的缓存功能

我们需要编辑proxy.c文件，在csapp.c和csapp.h文件中给我们提供了很多已经写好了的函数，在Readme中可以查看各函数的功能。当我们完成时，我们可以利用以下指令来进行跑分:
```
make clean
make
./driver.sh
```
需要注意的是，运行driver.sh可能会遇到无法解析的命令，我们需要安装对应的命令才行。


### 实验详解
在实现第一部分时，按照指导书的要求，我们要修改客户端发来的HTTP信息，例如将HTTP版本改为1.0，强制性修改连接为关闭，具体请阅读Writeup文档或直接阅读代码，然后转发给服务器。

对于URL的解析，发送给服务器请求行中的URL最终是以后缀形式(URI)，类似于: ```/home.html```(事实上这是被游览器优化的结果)，然后主机域名放在Host首部字段中，但客户端发送给代理服务器却使用标准URL甚至可能使用扩展的URL，例如```http://www.nc.com:8080/home.html```(当然端口或路径都可能为空，也有可能带参数)，这种情况下Host字段为空，在这种情况下我们要解析:
```
host: www.nc.com
port: 8080
path: /home.html
```

然后修改请求行和Host字段为:
```
GET /home.html HTTP/1.0
Host: www.nc.com
```
以标准形式发送给服务器。


在缓存中，我们需要用到读写者锁，即允许多个读者一起读缓存，只有有一个读者在读，就不允许写者写缓存，防止读者读入陈旧的信息，这就意味着我们需要两把锁，一把是基本的锁，一把是写者锁，我们用信号量来实现，具体的请看代码readcache和writecache开头和结尾部分。不过这里有个问题，即一直有读者来导致饿死写者，简单的解决方法是设置一个条件变量，读者必须等该变量为0时才能写，而一旦读者数量达到一定程度时且有写者等待，设置该变量为1，直到读者为0时，由写者释放它。（我没有实现）

对于实现近似RLU驱逐策略，我采用时钟思想，即为每一个缓存对象设置一个使用字段，该字段为1则代表最近使用，为0则代码未使用，我们还需要维护一个时钟指针，当缓存已满需要驱逐时，当前指针循环遍历，找到一个使用字段为0的对象，并驱逐它。如果对象的字段为1，那么将其置为0，继续向前遍历。这个思想大概是，如果当前字段为1，将当前置为0，下一次必须要经历一个轮回(循环)时间才能访问到该字段，当下一次访问到该字段时，如果其还为0，则代表这一个循环的时间内它都没有被使用过，则近似认为它是最近较少使用的对象，则可以驱逐它，反之该字段为1，则最近使用过，继续遍历。这仅仅只是一个近似的策略，肯定是没有那么高的效率的。


### 代码部分(十分详细，带解析)
```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_CACHE 10
/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/*关于读写者锁的结构体*/
struct rwlock_t{
	sem_t lock; 		//基本锁 
	sem_t writelock;	//写者所 
	int readers;		//读者个数 
};

/*关于近似LRU缓存的结构体*/
struct Cache{
	int  used;			//最近被使用过则为 1，否则为 0 
	char key[MAXLINE];	//URL索引 
	char value[MAX_OBJECT_SIZE];	//URL所对应的缓存 
};

/*关于url信息的结构体*/
struct UrlData{
	char host[MAXLINE];	//hostname
	char port[MAXLINE];	//端口 
	char path[MAXLINE];	//路径 
};

struct Cache cache[MAX_CACHE];	//定义缓存对象，最多允许存在MAX_CACHE个 
struct rwlock_t* rw;			//定义读写者锁指针 
int nowpointer;					//LRU当前指针 


void doit(int fd);
void parse_url(char* url, struct UrlData* u);		//解析URL 
void change_httpdata(rio_t* rio, struct UrlData* u, char* new_httpdata);	//修改http数据 
void thread(void* v);								//线程函数 
void rwlock_init();									//初始化读写者锁指针 
int  readcache(int fd, char* key);					//读缓存 
void writecache(char* buf, char* key);				//写缓存 

/*main函数大体与书上一致*/
int main(int argc, char** argv){
	rw = Malloc(sizeof(struct rwlock_t));
    printf("%s", user_agent_hdr);
	pthread_t tid;
	int listenfd, connfd;
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	
	rwlock_init();

	if (argc != 2){
		fprintf(stderr, "usage: %s <port>\n", argv[0]);	
		exit(0);
	}
	listenfd = Open_listenfd(argv[1]);
	while (1){
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
		Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s %s)\n", hostname, port);
		Pthread_create(&tid, NULL, thread, (void*)&connfd);
	}
    return 0;
}

void rwlock_init(){
	rw->readers = 0; 				//没有读者 
	sem_init(&rw->lock, 0, 1);
	sem_init(&rw->writelock, 0, 1);
}

void thread(void* v){
	int fd = *(int*)v; 				//这一步是必要的,v是main中connfd的地址，后续可能被改变，所以必须要得到一个副本
									//还要注意不能锁住，因为允许多个一起读 
	Pthread_detach(pthread_self()); //设置线程，结束后自动释放资源 
	doit(fd);
	close(fd);
}

void doit(int fd){
	char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
	char new_httpdata[MAXLINE], urltemp[MAXLINE];
	struct UrlData u;
	rio_t rio, server_rio;
	Rio_readinitb(&rio, fd);
	Rio_readlineb(&rio, buf, MAXLINE);

	sscanf(buf, "%s %s %s", method, url, version);
	strcpy(urltemp, url);	//赋值url副本以供读者写者使用，因为在解析url中，url可能改变 
	
	/*只接受GEI请求*/
	if (strcmp(method, "GET") != 0){
		printf ("The proxy can not handle this method: %s\n", method);
		return;
	}
	
	if (readcache(fd, urltemp) != 0)	//如果读者读取缓存成功的话，直接返回	
		return;

	parse_url(url, &u);		//解析url 
	change_httpdata(&rio, &u, new_httpdata);	//修改http数据，存入 new_httpdata中 
	
	int server_fd = Open_clientfd(u.host, u.port);
	size_t n;

	Rio_readinitb(&server_rio, server_fd);
	Rio_writen(server_fd, new_httpdata, strlen(new_httpdata));

	char cache[MAX_OBJECT_SIZE];
	int sum = 0;
	while((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0){
		Rio_writen(fd, buf, n);
		sum += n;
		strcat(cache, buf);
	}
	printf("proxy send %ld bytes to client\n", sum);
	if (sum < MAX_OBJECT_SIZE)
		writecache(cache, urltemp);	//如果可以的话，读入缓存 
	close(server_fd);
	return;
}

void writecache(char* buf, char* key){
	sem_wait(&rw->writelock);	//需要等待获得写者锁 
	int index;
	/*利用时钟算法，当前指针依次增加，寻找used字段为0的对象*/
	/*如果当前used为1，则设置为0，最坏情况下需要O(N)时间复杂度*/
	while (cache[nowpointer].used != 0){
		cache[nowpointer].used = 0;
		nowpointer = (nowpointer + 1) % MAX_CACHE;  
	}

	index = nowpointer;

	cache[index].used = 1;
	strcpy(cache[index].key, key);
	strcpy(cache[index].value, buf);
	sem_post(&rw->writelock);	//释放锁 
	return;
}

int readcache(int fd, char* url){
	sem_wait(&rw->lock);			//读者等待并获取锁(因为要修改全局变量，可能是线程不安全的，所以要锁) 
	if (rw->readers == 0)			//如果没有读者的话，说明可能有写者在写，必须等待并获取写者锁 
		sem_wait(&rw->writelock);	//读者再读时，不允许有写着 
	rw->readers++;
	sem_post(&rw->lock);			//全局变量修改完毕，接下来不会进入临界区，释放锁给更多读者使用 
	int i, flag = 0;
	
	/*依次遍历找到缓存，成功则设置返回值为 1*/
	for (i = 0; i < MAX_CACHE; i++){
		//printf ("Yes! %d\n",cache[i].usecnt);
		if (strcmp(url, cache[i].key) == 0){		
			Rio_writen(fd, cache[i].value, strlen(cache[i].value));
			printf("proxy send %d bytes to client\n", strlen(cache[i].value));
			cache[i].used = 1;
			flag = 1;
			break;
		}
	}

	sem_wait(&rw->lock);	/*进入临界区，等待并获得锁*/
	rw->readers--;			
	if (rw->readers == 0)	/*如果没有读者了，释放写者锁*/
		sem_post(&rw->writelock);
	sem_post(&rw->lock);	/*释放锁*/
	return flag;
}
/*解析url，解析为host, port, path*/
/*可能的情况: 
url: /home.html    这是目前大部分的形式，仅由路径构成，而host在Host首部字段中，端口默认80
url: http://www.xy.com:8080/home.html  这种情况下，Host首部字段为空，我们需要解析域名：www.xy.com,端口：8080,路径：/home.html*/
//该函数没有考虑参数等其他复杂情况 
void parse_url(char* url, struct UrlData* u){
	char* hostpose = strstr(url, "//");
	if (hostpose == NULL){
		char* pathpose = strstr(url, "/");
		if (pathpose != NULL)
			strcpy(u->path, pathpose);
		strcpy(u->port, "80");
		return;
	} else{
		char* portpose = strstr(hostpose + 2, ":");
		if (portpose != NULL){
			int tmp;
			sscanf(portpose + 1, "%d%s", &tmp, u->path);
			sprintf(u->port, "%d", tmp);
			*portpose = '\0';
			
		} else{
			char* pathpose = strstr(hostpose + 2, "/");
			if (pathpose != NULL){
				strcpy(u->path, pathpose);
				strcpy(u->port, "80");
				*pathpose = '\0'; 
			}
		}
		strcpy(u->host, hostpose + 2);
	}
	return;
}

void change_httpdata(rio_t* rio, struct UrlData* u, char* new_httpdata){
	static const char* Con_hdr = "Connection: close\r\n";
	static const char* Pcon_hdr = "Proxy-Connection: close\r\n";
	char buf[MAXLINE];
	char Reqline[MAXLINE], Host_hdr[MAXLINE], Cdata[MAXLINE];//分别为请求行，Host首部字段，和其他不东的数据信息 
	sprintf(Reqline, "GET %s HTTP/1.0\r\n", u->path);	//获取请求行 
	while (Rio_readlineb(rio, buf, MAXLINE) > 0){
		/*读到空行就算结束，GET请求没有实体体*/
		if (strcmp(buf, "\r\n") == 0){
			strcat(Cdata, "\r\n");
			break;			
		}
		else if (strncasecmp(buf, "Host:", 5) == 0){
			strcpy(Host_hdr, buf);
		}
		
		else if (!strncasecmp(buf, "Connection:", 11) && !strncasecmp(buf, "Proxy_Connection:", 17) &&!strncasecmp(buf, "User-agent:", 11)){
			strcat(Cdata, buf);
		}
	}
	if (!strlen(Host_hdr)){
		/*如果Host_hdr为空，说明该host被加载进请求行的URL中，我们格式读从URL中解析的host*/
		sprintf(Host_hdr, "Host: %s\r\n", u->host);	
	}
	
	sprintf(new_httpdata, "%s%s%s%s%s%s", Reqline, Host_hdr, Con_hdr, Pcon_hdr, user_agent_hdr, Cdata);
	return;
}

```

### 测试
键入命令以测试:
```
make clean
make
./driver.sh
```

![image.png](https://upload-images.jianshu.io/upload_images/26093099-af96c1a797db4676.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


##### 调试

建议使用curl调试，请参考writeup文档，例如你可以键入打开三个终端，两个运行代理和tiny服务器，一个键入命令：
```
./proxy 15214
```
```
./tiny 15213
```
```
linux> curl -v --proxy http://localhost15214 http://localhost15213/home.html
```
其中你可以修改/home.html为任何路径，例如
```
linux> curl -v --proxy http://localhost15214 http://localhost15213/csapp.h
```

### 配置火狐游览器
打开火狐游览器，找到设置一直下拉，找到网络设置，配置http代理，输入主机的ip地址与代理服务器监听的端口号即可。

咱们的代理只能处理GET哈哈哈。
![image.png](https://upload-images.jianshu.io/upload_images/26093099-0bb12684c9f5a4b8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

如果想让局域网内的其他主机访问改服务器，就得设置ip端口映射，通过物理机上的某个端口映射到该虚拟机上的服务器端口，具体的在虚拟机网络设置，NAT设置，添加里面，虚拟机ip和虚拟机端口要填写对应的。如果想让外网访问，就得进入路由器设置端口映射了。
