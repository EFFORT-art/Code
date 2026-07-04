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

    // 计数器，用于生成不重复的数字
    static int download_counter = 0;

    while (1) {
        char s[100];
        if (fgets(s, sizeof(s), stdin) == NULL)
            break;
        size_t len = strlen(s);
        if (len > 0 && s[len-1] == '\n')
            s[len-1] = '\0';
        if (strlen(s) == 0)
            continue;

        // 文件下载命令
        if (strncmp(s, "get ", 4) == 0) {
            write(client_fd, s, strlen(s));

            // 读取状态
            char status;
            read(client_fd, &status, 1);
            char dummy;
            read(client_fd, &dummy, 1);  // 跳过换行

            if (status == 'Y') {
                // 读取文件大小
                char size_buf[32];
                int i = 0;
                while (i < sizeof(size_buf) - 1) {
                    read(client_fd, &size_buf[i], 1);
                    if (size_buf[i] == '\n') break;
                    i++;
                }
                size_buf[i] = '\0';
                long file_size = atol(size_buf);

                // 生成本地文件名 download_计数器
                download_counter++;
                char download_filename[64];
                snprintf(download_filename, sizeof(download_filename), "download_%d", download_counter);

                FILE *fp = fopen(download_filename, "wb");
                if (fp) {
                    long remaining = file_size;
                    char buffer[1024];
                    while (remaining > 0) {
                        int to_read = (remaining > sizeof(buffer)) ? sizeof(buffer) : remaining;
                        int n_read = read(client_fd, buffer, to_read);
                        if (n_read <= 0) break;
                        fwrite(buffer, 1, n_read, fp);
                        remaining -= n_read;
                    }
                    fclose(fp);
                    printf("Downloaded %s\n", download_filename);
                } else {
                    printf("Failed to create local file\n");
                }
            } else {
                // 读掉错误信息（仅换行）
                while (1) {
                    char c;
                    read(client_fd, &c, 1);
                    if (c == '\n') break;
                }
                printf("File not found on server\n");
            }
            continue;
        }

        // 其他命令：发送并接收错误信息
        write(client_fd, s, strlen(s));
        char buf[100];
        int n = read(client_fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';
        printf("%s\n", buf);
    }

    close(client_fd);
    return 0;
}
