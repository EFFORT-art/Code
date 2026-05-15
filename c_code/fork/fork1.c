#include<stdio.h>
#include<unistd.h>

int main()
{
        for(int i=0;i<3;i++)
        {
             int a=fork();
             printf("%d\n",a);
        }
        return 0;
}
/*循环 n 次 fork，
  进程 2 的 n 次方，
  有多少子进程就有几个0
*/
