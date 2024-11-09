#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>  // 添加这行来支持bool类型

#ifdef __APPLE__
#include <sys/syslimits.h>
#elif defined(__linux__)
#include <linux/limits.h>
#elif defined(_WIN32)
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

// 平台类型
typedef enum {
    PLATFORM_WINDOWS,
    PLATFORM_MACOS,
    PLATFORM_LINUX,
    PLATFORM_UNKNOWN
} platform_t;

// 平台相关函数
platform_t get_platform(void);
const char* get_platform_name(void);
const char* get_default_rime_dir(void);
const char* get_default_backup_dir(void);
const char* get_config_path(void);
const char* get_rime_dir(void);
bool create_directory(const char *path);
bool copy_file(const char *src, const char *dst);
int execute_command(const char *cmd);
char* get_executable_path(void);

#endif // PLATFORM_H