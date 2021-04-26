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








