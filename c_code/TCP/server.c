#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <string.h>
#include<pthread.h>
#include<ctype.h>

void* handle_client(void* args)
{
	int conn_fd=*(int*)args;
	free(args);
	 while(1){
          char buf[100];
          int n=read(conn_fd,buf,sizeof(buf)-1);
          buf[n]='\0';
          for (int i = 0; i < n; i++) {
              buf[i] = toupper(buf[i]);
          }
 
          write(conn_fd,buf, n);
          }
}

int main()
{
	struct sockaddr_in addr;

	int listen_fd=socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(8000);	

	bind(listen_fd,(struct sockaddr*)&addr,sizeof(addr));
	listen(listen_fd,10);
		
	while(1)
	{
	int *p=malloc(sizeof(int));
	*p=accept(listen_fd,NULL,NULL);
	pthread_t tid;
	pthread_create(&tid,NULL,handle_client,p);
	pthread_detach(tid);
      	}
}
