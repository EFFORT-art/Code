#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define QUEUE_SIZE 10

// 循环队列结构
typedef struct {
    int data[QUEUE_SIZE];
    int front;   // 队头索引
    int rear;    // 队尾索引
    int size;    // 当前元素数量
} queue_t;

queue_t q = { .front = 0, .rear = 0, .size = 0 };

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

// 入队
int enqueue(int value) {
    if (q.size >= QUEUE_SIZE) return -1;
    q.data[q.rear] = value;
    q.rear = (q.rear + 1) % QUEUE_SIZE;
    q.size++;
    return 0;
}

// 出队
int dequeue(int *value) {
    if (q.size <= 0) return -1;
    *value = q.data[q.front];
    q.front = (q.front + 1) % QUEUE_SIZE;
    q.size--;
    return 0;
}

// 消费者
void* wait_thread(void* args) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (q.size <= 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }
        int value;
        dequeue(&value);
        printf("消费完成，值=%d，队列剩余大小=%d\n", value, q.size);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    return NULL;
}

// 生产者
void* signal_thread(void* args) {
    int item = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (q.size >= QUEUE_SIZE) {
            pthread_cond_wait(&not_full, &mutex);
        }
        item++;
        enqueue(item);
        printf("生产完成，值=%d，队列当前大小=%d\n", item, q.size);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    pthread_create(&t1, NULL, wait_thread, NULL);
    pthread_create(&t2, NULL, signal_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    return 0;
}
