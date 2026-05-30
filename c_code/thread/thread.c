#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void test(char* from)
{
    printf("from %s, pid %d, tid %lu\n", from, getpid(), pthread_self());
}

void* thread_func(void* args)
{
    test((char*)args);   
    return NULL;
}

int main()
{
    pthread_t tid1, tid2;  
    
    pthread_create(&tid1, NULL, thread_func, "thread1");
    
    pthread_create(&tid2, NULL, thread_func, "thread2");
    
    test("main");
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    return 0;
}
