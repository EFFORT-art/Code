#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO "fifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <目标文件>\n", argv[0]);
        return 1;
    }
    
    // 打开管道（读）
    int pipe = open(FIFO, O_RDONLY);
    
    // 创建目标文件
    int dst = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst == -1) {
        perror("创建文件失败");
        return 1;
    }
    
    // 从管道读取并写入文件
    char buf[1024];
    int n;
    while ((n = read(pipe, buf, 1024)) > 0) {
        write(dst, buf, n);
    }
    
    printf("接收完成\n");
    
    close(dst);
    close(pipe);
    unlink(FIFO);  // 删除管道文件
    return 0;
}
