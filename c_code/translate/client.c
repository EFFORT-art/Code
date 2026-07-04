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
    connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));

    char send_buf[256], recv_buf[256];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(send_buf, sizeof(send_buf), stdin))
            break;                     // EOF 或错误
        // 去掉换行符
        send_buf[strcspn(send_buf, "\n")] = '\0';
        if (strlen(send_buf) == 0) continue;

        write(client_fd, send_buf, strlen(send_buf));
        int n = read(client_fd, recv_buf, sizeof(recv_buf) - 1);
        if (n <= 0) break;
        recv_buf[n] = '\0';
        printf("%s\n", recv_buf);
    }
    close(client_fd);
    return 0;
}
