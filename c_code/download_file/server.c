#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

void* handle_client(void* args) {
    int conn_fd = *(int*)args;
    free(args);

    while (1) {
        char buf[100];
        int n = read(conn_fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';

        // ---- 文件下载命令 ----
        if (strncmp(buf, "get ", 4) == 0) {
            char *filename = buf + 4;
            FILE *fp = fopen(filename, "rb");
            if (fp) {
                fseek(fp, 0, SEEK_END);
                long size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                write(conn_fd, "Y\n", 2);
                char size_str[32];
                sprintf(size_str, "%ld\n", size);
                write(conn_fd, size_str, strlen(size_str));
                char buffer[1024];
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                    write(conn_fd, buffer, bytes);
                }
                fclose(fp);
            } else {
                write(conn_fd, "N\n", 2);
            }
            continue;
        }

        // ---- 其他命令：返回错误信息 ----
        char *err_msg = "Error: invalid command\n";
        write(conn_fd, err_msg, strlen(err_msg));
    }
    return NULL;
}

int main() {
    struct sockaddr_in addr;
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 10);

    while (1) {
        int *p = malloc(sizeof(int));
        *p = accept(listen_fd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, p);
        pthread_detach(tid);
    }
}
