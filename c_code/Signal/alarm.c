#include<unistd.h>
#include<stdio.h>

int main()
{
	alarm(2);
	while(1)
	{
		printf("hello world\n");
	}
}
