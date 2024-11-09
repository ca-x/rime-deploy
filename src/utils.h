#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// 添加对limits.h的引用
#ifdef __APPLE__
#include <sys/syslimits.h>
#elif defined(__linux__)
#include <linux/limits.h>
#elif defined(_WIN32)
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

// 日志级别
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

// 工具函数
bool init_logging(void);
void cleanup_logging(void);
void log_message(log_level_t level, const char *format, ...);
#define log_debug(...) log_message(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) log_message(LOG_INFO, __VA_ARGS__)
#define log_warning(...) log_message(LOG_WARNING, __VA_ARGS__)
#define log_error(...) log_message(LOG_ERROR, __VA_ARGS__)

char* path_join(const char *path1, const char *path2);
bool file_exists(const char *path);
bool dir_exists(const char *path);
char* read_file_content(const char *path);
bool write_file_content(const char *path, const char *content);

#endif // UTILS_H