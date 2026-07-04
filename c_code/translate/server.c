#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

/* ---------- 字典（Map）结构 ---------- */
#define MAX_ENTRIES 100
typedef struct {
    char key[64];
    char value[128];
} DictEntry;

DictEntry dict[MAX_ENTRIES];
int dict_size = 0;
pthread_mutex_t dict_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 预置一些翻译 */
void init_dict() {
    strcpy(dict[dict_size].key, "hello");   strcpy(dict[dict_size].value, "你好"); dict_size++;
    strcpy(dict[dict_size].key, "world");   strcpy(dict[dict_size].value, "世界"); dict_size++;
    strcpy(dict[dict_size].key, "apple");   strcpy(dict[dict_size].value, "苹果"); dict_size++;
    strcpy(dict[dict_size].key, "book");    strcpy(dict[dict_size].value, "书");   dict_size++;
}

/* 查找 key，返回 value 指针，找不到返回 NULL */
char* dict_find(const char* key) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(dict[i].key, key) == 0)
            return dict[i].value;
    }
    return NULL;
}

/* 更新或添加词条，成功返回 1，字典满返回 0 */
int dict_update(const char* key, const char* value) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(dict[i].key, key) == 0) {
            strcpy(dict[i].value, value);
            return 1;
        }
    }
    if (dict_size < MAX_ENTRIES) {
        strcpy(dict[dict_size].key, key);
        strcpy(dict[dict_size].value, value);
        dict_size++;
        return 1;
    }
    return 0;   // 字典已满
}

/* ---------- 客户端处理线程 ---------- */
void* handle_client(void* args) {
    int conn_fd = *(int*)args;
    free(args);

    char buf[256];   // 增大缓冲区，适应命令
    while (1) {
        int n = read(conn_fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;          // 客户端断开或出错
        buf[n] = '\0';

        // 去除末尾换行（如果有）
        if (buf[n-1] == '\n') buf[n-1] = '\0';

        // 解析命令：以 "update " 开头表示更新，否则视为查询
        if (strncmp(buf, "update ", 7) == 0) {
            char key[64], value[128];
            // 格式：update <key> <value>
            if (sscanf(buf + 7, "%63s %127s", key, value) == 2) {
                pthread_mutex_lock(&dict_mutex);
                int ok = dict_update(key, value);
                pthread_mutex_unlock(&dict_mutex);
                if (ok)
                    snprintf(buf, sizeof(buf), "已更新：%s -> %s", key, value);
                else
                    snprintf(buf, sizeof(buf), "字典已满，更新失败");
            } else {
                snprintf(buf, sizeof(buf), "格式错误，请用：update 单词 翻译");
            }
            write(conn_fd, buf, strlen(buf));
        } else {
            // 查询单词
            char* trans = NULL;
            pthread_mutex_lock(&dict_mutex);
            trans = dict_find(buf);
            pthread_mutex_unlock(&dict_mutex);
            if (trans) {
                snprintf(buf, sizeof(buf), "%s", trans);
            } else {
                snprintf(buf, sizeof(buf), "未找到该词");
            }
            write(conn_fd, buf, strlen(buf));
        }
    }
    close(conn_fd);
    return NULL;
}

/* ---------- 主服务器 ---------- */
int main() {
    init_dict();

    struct sockaddr_in addr;
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 10);

    while (1) {
        int* p = malloc(sizeof(int));
        *p = accept(listen_fd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, p);
        pthread_detach(tid);
    }
    close(listen_fd);
    return 0;
}
