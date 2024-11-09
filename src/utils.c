#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

// 日志文件句柄
static FILE* log_file = NULL;

// 初始化日志系统
bool init_logging(void) {
    char log_path[PATH_MAX];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // 创建logs目录
    #ifdef _WIN32
    mkdir("logs");
    #else
    mkdir("logs", 0755);
    #endif

    // 生成日志文件名
    snprintf(log_path, sizeof(log_path), "logs/rime_deploy_%04d%02d%02d_%02d%02d%02d.log",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    
    log_file = fopen(log_path, "w");
    return log_file != NULL;
}

// 清理日志系统
void cleanup_logging(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

// 写入日志
void log_message(log_level_t level, const char *format, ...) {
    const char *level_str[] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR"
    };
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    va_list args;
    va_start(args, format);
    
    // 输出到控制台
    if (level >= LOG_INFO) {
        fprintf(stderr, "[%s] %s: ", timestamp, level_str[level]);
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
    }
    
    // 输出到日志文件
    if (log_file) {
        fprintf(log_file, "[%s] %s: ", timestamp, level_str[level]);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
    
    va_end(args);
}

// 路径拼接
char* path_join(const char *path1, const char *path2) {
    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    char *result;
    
    // 分配内存空间
    result = malloc(len1 + len2 + 2); // +2 for separator and null terminator
    if (!result) return NULL;
    
    strcpy(result, path1);
    
    // 处理路径分隔符
    #ifdef _WIN32
    if (result[len1 - 1] != '\\' && path2[0] != '\\') {
        result[len1] = '\\';
        strcpy(result + len1 + 1, path2);
    } else {
        strcpy(result + len1, path2);
    }
    #else
    if (result[len1 - 1] != '/' && path2[0] != '/') {
        result[len1] = '/';
        strcpy(result + len1 + 1, path2);
    } else {
        strcpy(result + len1, path2);
    }
    #endif
    
    return result;
}

// 检查文件是否存在
bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

// 检查目录是否存在
bool dir_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

// 读取文件内容
char* read_file_content(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 分配内存
    char *content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // 读取文件内容
    if (fread(content, 1, size, file) != (size_t)size) {
        free(content);
        fclose(file);
        return NULL;
    }
    
    content[size] = '\0';
    fclose(file);
    return content;
}

// 写入文件内容
bool write_file_content(const char *path, const char *content) {
    FILE *file = fopen(path, "wb");
    if (!file) return false;
    
    size_t len = strlen(content);
    bool success = fwrite(content, 1, len, file) == len;
    
    fclose(file);
    return success;
}