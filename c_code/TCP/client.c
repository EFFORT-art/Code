#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>   
#include <string.h>

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));
   while(1){	
      	char s[100];
	scanf("%s",s);
	write(client_fd,s,strlen(s));
    char buf[100];

    int n = read(client_fd, buf, sizeof(buf) - 1);

    buf[n] = '\0';
    printf("%s\n", buf);
}
    close(client_fd);
    return 0;
}
