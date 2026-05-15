#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	//打开源文件
	int src_fd=open("src.txt",O_RDONLY);
	//打开目标文件
	int dest_fd=open("dest.txt",O_WRONLY | O_CREAT);
	//循环读入写入
	char buffer[1024];
	int bytes;
	while((bytes= read(src_fd, buffer, sizeof(buffer)))>0)
	{
		 write(dest_fd, buffer, bytes);
	}
	//关闭源文件
	close(src_fd);
	//关闭目标文件
	close(dest_fd);
        return 0;
}
