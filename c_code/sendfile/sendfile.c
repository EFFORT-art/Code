#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int main(int argc,char *argv[])
{
	if(argc!=3)
	   return 1;
	//打开源文件
	int src_fd=open(argv[1],O_RDONLY);
	if(src_fd==-1)
	{  
		perror("打开源文件失败");
		return 1;
	}
	
	//打开目标文件
	int dst_fd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
	 if(src_fd==-1)
         {
                  perror("打开源文件失败");
		  close(src_fd);
                  return 1;
 	 }

	//获取源文件大小
	struct stat stat_buf;
	if(fstat(src_fd,&stat_buf)==-1)
	{
		perror("sendfile 复制失败");
        	close(src_fd);
        	close(dst_fd);
		return 1;
	}

	//开始使用sendfile复制
	off_t offset=0;
	ssize_t bytes_sent=sendfile(dst_fd,src_fd,&offset,stat_buf.st_size);
		
	if (bytes_sent == -1) 
	{
        perror("sendfile 复制失败");
        close(src_fd);
        close(dst_fd);
        return 1;
     	}

	printf("成功复制 %ld 字节\n", bytes_sent);
    
    	// 关闭文件
    	close(src_fd);
    	close(dst_fd);
    
    	return 0;
}

