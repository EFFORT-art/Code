#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>      // 新增：用于文件操作

void* handle_client(void* args) {
    int conn_fd = *(int*)args;
    free(args);

    // 生成唯一的文件名（使用连接描述符）
    char filename[64];
    sprintf(filename, "upload_%d", conn_fd);

    // 打开本地文件用于写入
    int out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("open");
        close(conn_fd);
        return NULL;
    }

    char buf[1024];
    ssize_t n;
    while ((n = read(conn_fd, buf, sizeof(buf))) > 0) {
        write(out_fd, buf, n);   // 写入文件
    }

    close(out_fd);
    close(conn_fd);
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
