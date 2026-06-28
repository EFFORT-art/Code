#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <string.h>

int main()
{
	struct sockaddr_in addr;

	int listen_fd=socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(8000);	

	bind(listen_fd,(struct sockaddr*)&addr,sizeof(addr));
	listen(listen_fd,10);
		
	while(1){
	int conn_fd=accept(listen_fd,NULL,NULL);
	char *http_resp = 
  	  "HTTP/1.1 200 OK\r\n"
    	"Content-Type: text/plain\r\n"
    	"Content-Length: 5\r\n"
    	"\r\n"
    	"hello";
	write(conn_fd, http_resp, strlen(http_resp));
	close(conn_fd);
	}
}
