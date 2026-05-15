#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/**
 * 使用系统调用实现文件拷贝
 * @param src_path 源文件路径
 * @param dest_path 目标文件路径
 * @return 成功返回0，失败返回-1
 */
int file_copy(const char *src_path, const char *dest_path) {
    int src_fd = -1, dest_fd = -1;
    char buffer[4096];  // 4KB缓冲区
    ssize_t bytes_read, bytes_written;
    struct stat src_stat;
    
    // 打开源文件（只读模式）
    src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        fprintf(stderr, "无法打开源文件 '%s': %s\n", src_path, strerror(errno));
        return -1;
    }
    
    // 获取源文件信息
    if (fstat(src_fd, &src_stat) == -1) {
        fprintf(stderr, "无法获取源文件信息: %s\n", strerror(errno));
        close(src_fd);
        return -1;
    }
    
    // 创建目标文件（读写模式，如果存在则截断，设置权限）
    dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dest_fd == -1) {
        fprintf(stderr, "无法创建目标文件 '%s': %s\n", dest_path, strerror(errno));
        close(src_fd);
        return -1;
    }
    
    printf("开始拷贝: %s -> %s\n", src_path, dest_path);
    
    // 循环读取和写入数据
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        char *write_ptr = buffer;
        ssize_t total_written = 0;
        
        // 确保所有数据都被写入
        while (total_written < bytes_read) {
            bytes_written = write(dest_fd, write_ptr + total_written, 
                                  bytes_read - total_written);
            if (bytes_written == -1) {
                if (errno == EINTR) {
                    continue;  // 被信号中断，重试
                }
                fprintf(stderr, "写入文件失败: %s\n", strerror(errno));
                close(src_fd);
                close(dest_fd);
                return -1;
            }
            total_written += bytes_written;
        }
    }
    
    // 检查读取是否出错
    if (bytes_read == -1) {
        fprintf(stderr, "读取文件失败: %s\n", strerror(errno));
        close(src_fd);
        close(dest_fd);
        return -1;
    }
    
    // 同步文件数据到磁盘（在关闭之前）
    if (fsync(dest_fd) == -1) {
        fprintf(stderr, "同步文件失败: %s\n", strerror(errno));
        // 这里不返回错误，因为拷贝已经完成
    }
    
    // 关闭文件描述符
    close(src_fd);
    close(dest_fd);
    
    printf("文件拷贝完成\n");
    return 0;
}

void print_usage(const char *program_name) {
    printf("用法: %s <源文件> <目标文件>\n", program_name);
    printf("示例: %s file1.txt file2.txt\n", program_name);
}

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 3) {
        fprintf(stderr, "错误: 需要两个参数\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *src_path = argv[1];
    const char *dest_path = argv[2];
    
    // 检查源文件和目标文件是否相同
    if (strcmp(src_path, dest_path) == 0) {
        fprintf(stderr, "错误: 源文件和目标文件不能相同\n");
        return EXIT_FAILURE;
    }
    
    // 执行文件拷贝
    if (file_copy(src_path, dest_path) == 0) {
        printf("拷贝成功\n");
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "拷贝失败\n");
        return EXIT_FAILURE;
    }
}
