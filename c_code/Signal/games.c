#include<unistd.h>
#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>

void game_timeout(int sig)
{
	printf("time out!Sig:%d\n",sig);
	exit(0);
}

int main()
{
	printf("5+2=?\n");
	
	alarm(5);

	struct sigaction sa;
	sa.sa_handler = game_timeout;
	sigemptyset(&sa.sa_mask);	
	sa.sa_flags=0;

	sigaction(SIGALRM,&sa,NULL);
	
	char answer[100];
	fgets(answer,100,stdin);
	answer[strcspn(answer,"\n")]=0;
	if(strcmp(answer,"7")==0)
	{
		printf("yes\n");
	}
	else
	{
		printf("error\n");
	}


	return 0;
}
