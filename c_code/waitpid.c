#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // 创建3个子进程
    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            sleep(i + 1);
            printf("子进程 %d 退出\n", i + 1);
            return 0;
        }
    }

    // 非阻塞等待所有子进程
    while (1) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);  // -1 表示等待任意子进程
        
        if (pid > 0) {
            printf("父进程：子进程 %d 退出了\n", pid);
        } else if (pid == 0) {
            printf("父进程：还有子进程在运行，做点别的事...\n");
            sleep(1);  // 过会儿再检查
        } else {
            break;  // 没有子进程了
        }
    }
    
    printf("所有子进程结束\n");
    return 0;
}
