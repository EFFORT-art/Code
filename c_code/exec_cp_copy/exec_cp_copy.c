#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 检查参数
    if (argc != 3) {
        fprintf(stderr, "用法: %s <源文件> <目标文件>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0) {
        // 子进程：执行 cp 命令
        printf("子进程开始拷贝: %s -> %s\n", argv[1], argv[2]);
        
       	// 方法1：使用 execlp 执行 
        // execlp("cp", "cp", argv[1], argv[2], NULL);
        
        // 方法2：使用 execl
        execl("/bin/cp", "cp", argv[1], argv[2], NULL);
        
        // 如果 exec 失败
        perror("exec failed");
        return 1;
    }
    else {
        // 父进程：等待子进程完成
        int status;
        wait(&status);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("文件拷贝成功！\n");
        } else {
            printf("文件拷贝失败！\n");
        }
    }
    
    return 0;
}
