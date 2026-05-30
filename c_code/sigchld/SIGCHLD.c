#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int signo) {
    printf("信号处理函数被内核自动调用了！\n");
    waitpid(-1, NULL, WNOHANG);
}

int main() {
    signal(SIGCHLD, sigchld_handler);
    
    if (fork() == 0) {
        printf("子进程退出\n");
        return 0;
    }
    
    sleep(1);  // 等待信号
    printf("父进程结束\n");
    return 0;
}
