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
struct rwlock_t{
	sem_t lock;
	sem_t writelock;
	int readers;
};

struct Cache{
	int  used;
	char key[MAXLINE];
	char value[MAX_OBJECT_SIZE];
};

struct UrlData{
	char host[MAXLINE];
	char port[MAXLINE];
	char path[MAXLINE];
};

struct Cache cache[MAX_CACHE];
struct rwlock_t* rw;
int nowpointer;


void doit(int fd);
void parse_url(char* url, struct UrlData* u);
void change_httpdata(rio_t* rio, struct UrlData* u, char* new_httpdata);
void thread(void* v);
void rwlock_init();
int  readcache(int fd, char* key);
void writecache(char* buf, char* key);

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
	rw->readers = 0;
	sem_init(&rw->lock, 0, 1);
	sem_init(&rw->writelock, 0, 1);
}

void thread(void* v){
	int fd = *(int*)v; 
	Pthread_detach(pthread_self());
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
	strcpy(urltemp, url);

	if (strcmp(method, "GET") != 0){
		printf ("The proxy can not handle this method: %s\n", method);
		return;
	}
	if (readcache(fd, urltemp) != 0)		
		return;

	parse_url(url, &u);
	change_httpdata(&rio, &u, new_httpdata);
	
	int server_fd = Open_clientfd(u.host, u.port);
	size_t n;

	Rio_readinitb(&server_rio, server_fd);
	Rio_writen(server_fd, new_httpdata, strlen(new_httpdata));

	char cache[MAX_OBJECT_SIZE];
	int sum = 0;
	while((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0){
		Rio_writen(fd, buf, n);
		sum += n;
		if (sum < MAX_OBJECT_SIZE)
			strcat(cache, buf);
	}
	printf("proxy send %ld bytes to client\n", sum);
	//if (sum < MAX_OBJECT_SIZE)
		writecache(cache, urltemp);
	close(server_fd);
	return;
}

void writecache(char* buf, char* key){
	sem_wait(&rw->writelock);
	int index;
	while (cache[nowpointer].used != 0){
		cache[nowpointer].used = 0;
		nowpointer = (nowpointer + 1) % MAX_CACHE;  
	}

	index = nowpointer;

	cache[index].used = 1;
	strcpy(cache[index].key, key);
	strcpy(cache[index].value, buf);
	sem_post(&rw->writelock);
	return;
}

int readcache(int fd, char* url){
	sem_wait(&rw->lock);
	if (rw->readers == 0)
		sem_wait(&rw->writelock);
	rw->readers++;
	sem_post(&rw->lock);
	int i, flag = 0;
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

	sem_wait(&rw->lock);
	rw->readers--;
	if (rw->readers == 0)
		sem_post(&rw->writelock);
	sem_post(&rw->lock);
	return flag;
}
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
	char Reqline[MAXLINE], Host_hdr[MAXLINE], Cdata[MAXLINE];
	sprintf(Reqline, "GET %s HTTP/1.0\r\n", u->path);
	while (Rio_readlineb(rio, buf, MAXLINE) > 0){
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
		sprintf(Host_hdr, "Host: %s\r\n", u->host);	
	}
	
	sprintf(new_httpdata, "%s%s%s%s%s%s", Reqline, Host_hdr, Con_hdr, Pcon_hdr, user_agent_hdr, Cdata);
	return;
}








