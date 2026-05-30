#include<pthread.h>
#include<stdlib.h>
#include<string.h>


void* sell(void* args)
{
	char* p=(char*)args;
	while(ticket_count<100)
	{
		printf("%s售出第%d张票"，p,)
	}
}


int main()
{
	pthread_t tid1,tid2;
	pthread_create(&tid1,NULL,sell,"window 1");
	pthread_create(&tid2,NULL,sell,"window 2");
        pthread_join(tid1,NULL);
	pthrerad_join(tid2,NULL);
	return 0;
}	
