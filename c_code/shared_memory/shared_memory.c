#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define SHM_SIZE 128

int main() {
    // 创建匿名共享内存（父子进程共享）
    char *shared = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：稍等一下让父进程先写
        sleep(1);
        printf("子进程读到: %s\n", shared);
        munmap(shared, SHM_SIZE);
        exit(0);
    } else {
        // 父进程：写入数据
        strcpy(shared, "Hello from parent!");
        printf("父进程已写入: %s\n", shared);
        wait(NULL);          // 等待子进程结束
        munmap(shared, SHM_SIZE);
    }
    return 0;
}
