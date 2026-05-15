#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n = 3;  // 创建3个子进程

    // 创建子进程
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // 子进程
            printf("子进程 %d (PID=%d) 开始\n", i+1, getpid());
            sleep(i+1);  // 每个子进程工作不同时间
            printf("子进程 %d (PID=%d) 结束\n", i+1, getpid());
            return 0;  // 子进程结束
        }
    }

    // 父进程：等待所有子进程
    printf("父进程等待 %d 个子进程结束...\n", n);
    
    for (int i = 0; i < n; i++) {
        wait(NULL);  // NULL表示不关心子进程的退出状态
        printf("父进程：第 %d 个子进程结束了\n", i+1);
    }

    printf("所有子进程都结束了，父进程退出\n");
    return 0;
}
