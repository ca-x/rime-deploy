#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include "platform.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#define PATH_SEP "\\"
#else
#include <unistd.h>
#include <pwd.h>
#define PATH_SEP "/"
#endif

// 获取平台类型
platform_t get_platform(void) {
#ifdef _WIN32
    return PLATFORM_WINDOWS;
#elif defined(__APPLE__)
    return PLATFORM_MACOS;
#elif defined(__linux__)
    return PLATFORM_LINUX;
#else
    return PLATFORM_UNKNOWN;
#endif
}

// 获取平台名称
const char* get_platform_name(void) {
    switch(get_platform()) {
        case PLATFORM_WINDOWS: return "Windows";
        case PLATFORM_MACOS: return "macOS";
        case PLATFORM_LINUX: return "Linux";
        default: return "Unknown";
    }
}

// 获取用户主目录
static const char* get_home_dir(void) {
#ifdef _WIN32
    return getenv("USERPROFILE");
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd) {
            home = pwd->pw_dir;
        }
    }
    return home;
#endif
}

// 获取RIME默认配置目录
const char* get_rime_dir(void) {
    static char rime_dir[PATH_MAX] = {0};
    if (rime_dir[0] != '\0') {
        return rime_dir;
    }

    const char* home = get_home_dir();
    if (!home) {
        log_error("Failed to get home directory");
        return NULL;
    }

    platform_t platform = get_platform();
    switch(platform) {
        case PLATFORM_WINDOWS:
            snprintf(rime_dir, PATH_MAX, "%s\\AppData\\Roaming\\Rime", home);
            break;
        case PLATFORM_MACOS:
            snprintf(rime_dir, PATH_MAX, "%s/Library/Rime", home);
            break;
        case PLATFORM_LINUX:
            // 检测输入法框架
            if (dir_exists("/usr/share/fcitx5")) {
                snprintf(rime_dir, PATH_MAX, "%s/.local/share/fcitx5/rime", home);
            } else if (dir_exists("/usr/share/fcitx")) {
                snprintf(rime_dir, PATH_MAX, "%s/.config/fcitx/rime", home);
            } else if (dir_exists("/usr/share/ibus")) {
                snprintf(rime_dir, PATH_MAX, "%s/.config/ibus/rime", home);
            } else {
                log_error("No supported input framework found");
                return NULL;
            }
            break;
        default:
            log_error("Unsupported platform");
            return NULL;
    }
    return rime_dir;
}

// 获取默认备份目录
const char* get_default_backup_dir(void) {
    static char backup_dir[PATH_MAX];
    const char* rime_dir = get_rime_dir();
    if (!rime_dir) return NULL;

    snprintf(backup_dir, PATH_MAX, "%s%sbackup", rime_dir, PATH_SEP);
    return backup_dir;
}

// 创建目录
bool create_directory(const char *path) {
#ifdef _WIN32
    return CreateDirectoryA(path, NULL) ||
           GetLastError() == ERROR_ALREADY_EXISTS;
#else
    return mkdir(path, 0755) == 0 ||
           errno == EEXIST;
#endif
}

// 复制文件
bool copy_file(const char *src, const char *dst) {
#ifdef _WIN32
    return CopyFileA(src, dst, FALSE);
#else
    FILE *source = fopen(src, "rb");
    if (!source) return false;

    FILE *dest = fopen(dst, "wb");
    if (!dest) {
        fclose(source);
        return false;
    }

    char buf[8192];
    size_t size;

    while ((size = fread(buf, 1, sizeof(buf), source)) > 0) {
        if (fwrite(buf, 1, size, dest) != size) {
            fclose(source);
            fclose(dest);
            return false;
        }
    }

    fclose(source);
    fclose(dest);
    return true;
#endif
}

// 执行系统命令
int execute_command(const char *cmd) {
#ifdef _WIN32
    return system(cmd);
#else
    return system(cmd);
#endif
}

// 获取可执行文件路径
char* get_executable_path(void) {
    static char path[PATH_MAX];
#ifdef _WIN32
    if (!GetModuleFileNameA(NULL, path, PATH_MAX)) {
        return NULL;
    }
#elif defined(__APPLE__)
    uint32_t size = PATH_MAX;
    if (_NSGetExecutablePath(path, &size) != 0) {
        return NULL;
    }
#else
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path)-1);
    if (len == -1) {
        return NULL;
    }
    path[len] = '\0';
#endif
    return path;
}

// 获取配置文件路径
const char* get_config_path(void) {
    static char config_path[PATH_MAX];
    const char* rime_dir = get_rime_dir();
    if (!rime_dir) return NULL;

    snprintf(config_path, PATH_MAX, "%s%srime.conf", rime_dir, PATH_SEP);
    return config_path;
}