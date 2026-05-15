#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int fd1[2];  // 父→子 管道
    int fd2[2];  // 子→父 管道
    pipe(fd1);
    pipe(fd2);
    
    char buf1[1024] = "hello world";
    char buf2[1024] = "ni hao";
    char buffer[1024];
    
    if (fork() == 0)  // 子进程
    {
        // 关闭不需要的端口
        close(fd1[1]);  // 关闭 fd1 写端（不用发送）
        close(fd2[0]);  // 关闭 fd2 读端（不用接收）
        
        // 从 fd1 读取父进程的消息
        read(fd1[0], buffer, sizeof(buffer));
        printf("子进程收到：%s\n", buffer);
        
        // 向 fd2 发送消息给父进程
        write(fd2[1], buf2, strlen(buf2) + 1);
        printf("子进程发送：%s\n", buf2);
        
        // 关闭端口
        close(fd1[0]);
        close(fd2[1]);
    }
    else  // 父进程
    {
        // 关闭不需要的端口
        close(fd1[0]);  // 关闭 fd1 读端（不用接收）
        close(fd2[1]);  // 关闭 fd2 写端（不用发送）
        
        // 向 fd1 发送消息给子进程
        write(fd1[1], buf1, strlen(buf1) + 1);
        printf("父进程发送：%s\n", buf1);
        
        // 从 fd2 读取子进程的回复
        read(fd2[0], buffer, sizeof(buffer));
        printf("父进程收到：%s\n", buffer);
        
        // 关闭端口
        close(fd1[1]);
        close(fd2[0]);
        
        // 等待子进程结束
        wait(NULL);
    }
    return 0;
}
