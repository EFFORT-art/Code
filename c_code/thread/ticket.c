#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 总票数
int tickets = 100;
// 添加互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 售票线程函数
void* sell_ticket(void* arg) {
    char* thread_name = (char*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (tickets > 0) {
            printf("%s售出第%d张票\n", thread_name, tickets);
            tickets--;
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    printf("%s票已售完，剩余：%d张门票\n", thread_name, tickets);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    printf("开始售票，总票数：%d\n", tickets);
    
    // 创建两个售票线程
    pthread_create(&thread1, NULL, sell_ticket, "窗口1");
    pthread_create(&thread2, NULL, sell_ticket, "窗口2");
    
    // 等待线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("所有票已售完，最终剩余：%d\n", tickets);
    return 0;
}
