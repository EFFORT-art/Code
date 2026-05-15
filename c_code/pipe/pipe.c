#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
	int fd[2];
	pipe(fd);
	char buf[1024]="hello world";
	if(fork()==0)
	{	
		close(fd[1]);
		read(fd[0],buf,sizeof(buf));
		printf("子进程已读取：%s\n",buf);
		close(fd[0]);
	}else{
		close(fd[0]);
		write(fd[1],buf,sizeof(buf));
		printf("父进程已写入\n");
		wait(NULL);
		close(fd[1]);
	}
	return 0;
}
