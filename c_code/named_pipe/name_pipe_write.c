#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO "fifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <源文件>\n", argv[0]);
        return 1;
    }
    
    // 创建管道
    mkfifo(FIFO, 0666);
    
    // 打开源文件
    int src = open(argv[1], O_RDONLY);
    if (src == -1) {
        perror("打开文件失败");
        return 1;
    }
    
    // 打开管道（写）
    int pipe = open(FIFO, O_WRONLY);
    
    // 读取源文件并写入管道
    char buf[1024];
    int n;
    while ((n = read(src, buf, 1024)) > 0) {
        write(pipe, buf, n);
    }
    
    printf("发送完成\n");
    
    close(src);
    close(pipe);
    return 0;
}
