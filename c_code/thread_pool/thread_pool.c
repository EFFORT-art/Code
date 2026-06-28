#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

/* ---------- 配置参数 ---------- */
#define NUM_THREADS     5       // 消费者线程数
#define QUEUE_CAPACITY  10      // 任务队列最大容量

/* ---------- 任务结构 ---------- */
typedef struct Task {
    void (*function)(void*);
    void* arg;
} Task;

/* ---------- 线程池结构 ---------- */
typedef struct ThreadPool {
    pthread_t* threads;
    int thread_count;
    Task* task_queue;
    int front;
    int rear;
    int size;
    int capacity;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    int shutdown;
} ThreadPool;

static ThreadPool pool;

/* ---------- 工作线程函数 ---------- */
void* worker(void* arg) {
    while (1) {
        pthread_mutex_lock(&pool.mutex);
        while (pool.size == 0 && !pool.shutdown) {
            pthread_cond_wait(&pool.not_empty, &pool.mutex);
        }
        if (pool.shutdown && pool.size == 0) {
            pthread_mutex_unlock(&pool.mutex);
            break;
        }

        Task task = pool.task_queue[pool.front];
        pool.front = (pool.front + 1) % pool.capacity;
        pool.size--;
        int remaining = pool.size;   // 取出后队列剩余任务数

        pthread_cond_signal(&pool.not_full);
        pthread_mutex_unlock(&pool.mutex);

        // 消费输出（包含剩余任务数）
        printf("消费者线程 %lu 取出任务，队列剩余任务数 = %d\n", pthread_self(), remaining);

        // 执行任务
        (task.function)(task.arg);
    }
    return NULL;
}

/* ---------- 线程池初始化 ---------- */
void pool_init(int num_threads, int queue_capacity) {
    pool.thread_count = num_threads;
    pool.capacity = queue_capacity;
    pool.task_queue = (Task*)malloc(sizeof(Task) * queue_capacity);
    pool.front = pool.rear = pool.size = 0;
    pool.shutdown = 0;

    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.not_empty, NULL);
    pthread_cond_init(&pool.not_full, NULL);

    pool.threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool.threads[i], NULL, worker, NULL);
    }
}

/* ---------- 提交任务（方式二核心） ---------- */
int pool_submit(void (*function)(void*), void* arg) {
    pthread_mutex_lock(&pool.mutex);

    while (pool.size == pool.capacity && !pool.shutdown) {
        pthread_cond_wait(&pool.not_full, &pool.mutex);
    }
    if (pool.shutdown) {
        pthread_mutex_unlock(&pool.mutex);
        return -1;
    }

    Task new_task = { function, arg };
    pool.task_queue[pool.rear] = new_task;
    pool.rear = (pool.rear + 1) % pool.capacity;
    pool.size++;

    pthread_cond_signal(&pool.not_empty);
    pthread_mutex_unlock(&pool.mutex);
    return 0;
}

/* ---------- 销毁线程池 ---------- */
void pool_destroy() {
    pthread_mutex_lock(&pool.mutex);
    pool.shutdown = 1;
    pthread_cond_broadcast(&pool.not_empty);
    pthread_mutex_unlock(&pool.mutex);

    for (int i = 0; i < pool.thread_count; i++) {
        pthread_join(pool.threads[i], NULL);
    }

    free(pool.threads);
    free(pool.task_queue);
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.not_empty);
    pthread_cond_destroy(&pool.not_full);
}

/* ---------- 示例任务：消费一个整数 ---------- */
void consume_int(void* arg) {
    int id = *(int*)arg;
    free(arg);
    printf("  消费 ID = %d\n", id);
    sleep(2);   // 消费慢一点
}

/* ---------- 主程序（修改后：去掉休息3秒） ---------- */
int main() {
    // 1. 初始化线程池（5个消费者，队列容量10）
    pool_init(NUM_THREADS, QUEUE_CAPACITY);

    // 2. 生产者循环：连续生产20个ID，每0.5秒一个，不休息
    int id = 0;
    for (int i = 0; i < 20; i++) {
        id++;
        int* arg = (int*)malloc(sizeof(int));
        *arg = id;

        if (pool_submit(consume_int, arg) == 0) {
            // 提交成功后，加锁读取当前队列中的任务数（即“当前容量”）
            pthread_mutex_lock(&pool.mutex);
            int cur_size = pool.size;
            pthread_mutex_unlock(&pool.mutex);
            printf("生产 ID = %d，当前队列任务数 = %d\n", id, cur_size);
        }

        // 生产快一点：每0.5秒生产一个
        usleep(500000);   // 0.5秒
    }

    // 3. 等待所有任务处理完成
    printf("所有任务已提交，等待消费完成...\n");
    sleep(10);   // 预留足够时间

    // 4. 销毁线程池
    pool_destroy();
    printf("线程池已销毁。\n");
    return 0;
}
