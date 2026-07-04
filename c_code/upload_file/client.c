#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>     

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_to_upload>\n", argv[0]);
        exit(1);
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));

    // 打开待上传的文件
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("fopen");
        close(client_fd);
        exit(1);
    }

    char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        write(client_fd, buffer, n);
    }

    fclose(fp);
    close(client_fd);   // 关闭连接
    return 0;
}
