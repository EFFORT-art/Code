#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define THREAD_NUM 4   //线程数
#define BUFSIZE 1024   //读写缓冲区大小

//结构体:向线程传递参数 
typedef struct 
{	 
	 off_t off;  //偏移量 
 	 size_t len; //需要拷贝的总字节数
	 int id; //线程编号
} job;

void* work(void* arg) {
    job *j = (job*)arg;
    char buf[BUFSIZE];
    int src = open("simple.txt", O_RDONLY);
    int dst = open("dest.txt", O_WRONLY);
    lseek(src, j->off, SEEK_SET);  //将指针移到偏移位置，即每个线程开始写的位置
    lseek(dst, j->off, SEEK_SET);
    size_t remain = j->len;  //记录剩余为拷贝字节数
    while (remain) {
        size_t r = (remain < BUFSIZE) ? remain : BUFSIZE;
        int n = read(src, buf, r);
        if (n <= 0) break;  //若读取失败退出循环，n返回读取到的字节数
        write(dst, buf, n);
        remain -= n;
    }
    close(src); close(dst);
    return NULL;
}

int main() {
    struct stat st;  //存储文件状态的结构体
    stat("simple.txt", &st);//获取状态
    off_t size = st.st_size; //获取文件大小，字节数

    int dst = open("dest.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);  //O_TRUNC清空文件
    ftruncate(dst, size);  //预留size大小的文件空间
    close(dst);

    pthread_t tid[THREAD_NUM];  //存各个线程的ID
    job jobs[THREAD_NUM];       //存各个线程的参数
    size_t chunk = (size + THREAD_NUM - 1) / THREAD_NUM;//向上取整防止截断

    for (int i = 0; i < THREAD_NUM; i++) {
        off_t start = i * chunk;
        size_t len = (start + chunk <= size) ? chunk : (size - start);//计算需要拷贝的长度
        if (len == 0) break;//拷贝完退出循环
        jobs[i] = (job){start, len, i};
        pthread_create(&tid[i], NULL, work, &jobs[i]);
    }
    for (int i = 0; i < THREAD_NUM; i++) pthread_join(tid[i], NULL);       //阻塞至都退出
    return 0;
}
