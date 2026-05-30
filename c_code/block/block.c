#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask, pending;   // mask: 阻塞信号集, pending: 未决信号集
    
    // 1. 清空 mask 集合
    sigemptyset(&mask);
    // 2. 将 SIGINT(2号信号) 加入 mask 集合
    sigaddset(&mask, SIGINT);
    // 3. 设置阻塞掩码：阻塞 mask 中的信号（即阻塞 SIGINT）
    //    SIG_BLOCK 表示把 mask 中的信号添加到当前阻塞集中
    sigprocmask(SIG_BLOCK, &mask, NULL);
    
    // 4. 产生 SIGINT 信号（向自己发送）
    //    此时信号被阻塞，所以进入【未决】状态，pending 位被设为 1
    printf("产生 SIGINT，按图中 pending 位变 1\n");
    raise(SIGINT);
    
    // 5. 获取当前未决信号集，存入 pending
    sigpending(&pending);
    // 6. 检查 SIGINT 是否在未决集中
    //    返回 1 表示是，0 表示否
    printf("pending 中 SIGINT=%d (1表示未决)\n", 
           sigismember(&pending, SIGINT));
    
    // 7. 解除对 SIGINT 的阻塞
    //    此时未决的 SIGINT 会立即【递达】
    //    SIGINT 默认动作是终止进程，所以进程在这里会退出
    printf("解除阻塞，进程将终止\n");
    sleep(1);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    
    // 8. 这行不会执行，因为进程已经在上面被终止了
    printf("这行不会打印\n");
    return 0;
}
