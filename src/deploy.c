#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include "deploy.h"
#include "platform.h"
#include "utils.h"

// 安装RIME输入法
bool install_rime(void) {
    log_info("Installing RIME input method...");
    
    platform_t platform = get_platform();
    switch(platform) {
        case PLATFORM_MACOS:
            if (execute_command("brew install --cask squirrel") != 0) {
                log_error("Failed to install Squirrel");
                return false;
            }
            break;
            
        case PLATFORM_LINUX:
            // 检测包管理器
            if (file_exists("/usr/bin/apt")) {
                if (execute_command("sudo apt install fcitx-rime -y") != 0) {
                    log_error("Failed to install fcitx-rime");
                    return false;
                }
            } else if (file_exists("/usr/bin/dnf")) {
                if (execute_command("sudo dnf install rime-fcitx -y") != 0) {
                    log_error("Failed to install rime-fcitx");
                    return false;
                }
            } else {
                log_error("Unsupported Linux distribution");
                return false;
            }
            break;
            
        case PLATFORM_WINDOWS:
            log_info("Please install RIME manually on Windows");
            log_info("Download from: https://rime.im/download/");
            return true;
            
        default:
            log_error("Unsupported platform");
            return false;
    }
    
    return true;
}

// 备份现有配置
bool backup_config(void) {
    log_info("Backing up existing configuration...");
    
    const char* rime_dir = get_rime_dir();
    if (!rime_dir || !dir_exists(rime_dir)) {
        log_info("No existing configuration to backup");
        return true;
    }
    
    // 生成备份目录名
    time_t now = time(NULL);
    char backup_path[PATH_MAX];
    snprintf(backup_path, sizeof(backup_path), "%s.old.%ld", rime_dir, now);
    
    // 执行备份
    if (rename(rime_dir, backup_path) != 0) {
        log_error("Failed to backup configuration");
        return false;
    }
    
    log_info("Configuration backed up to: %s", backup_path);
    return true;
}

// 安装雾凇拼音配置
bool install_rime_ice(void) {
    log_info("Installing Rime-ice configuration...");
    
    // 创建临时目录
    const char* temp_dir = "/tmp/rime-ice";
    if (dir_exists(temp_dir)) {
        execute_command("rm -rf /tmp/rime-ice");
    }
    
    // 克隆配置仓库
    if (execute_command("git clone --depth=1 https://github.com/iDvel/rime-ice.git /tmp/rime-ice") != 0) {
        log_error("Failed to download Rime-ice configuration");
        return false;
    }
    
    // 复制配置文件
    const char* rime_dir = get_rime_dir();
    if (!rime_dir) {
        log_error("Failed to get RIME directory");
        return false;
    }
    
    // 确保目录存在
    if (!dir_exists(rime_dir)) {
        if (!create_directory(rime_dir)) {
            log_error("Failed to create RIME directory");
            return false;
        }
    }
    
    // 复制配置文件
    char cmd[PATH_MAX * 2];
    snprintf(cmd, sizeof(cmd), "cp -r %s/* %s/", temp_dir, rime_dir);
    if (execute_command(cmd) != 0) {
        log_error("Failed to copy Rime-ice configuration");
        return false;
    }
    
    // 清理临时文件
    execute_command("rm -rf /tmp/rime-ice");
    
    log_info("Rime-ice configuration installed successfully");
    return true;
}

// 应用自定义配置
bool apply_custom_config(void) {
    log_info("Applying custom configuration...");
    
    // 检查custom目录是否存在
    struct stat st;
    if (stat("custom", &st) != 0 || !S_ISDIR(st.st_mode)) {
        log_info("Custom directory does not exist, skipping custom config");
        return true;  // 不存在custom目录不算错误，继续执行
    }
    
    // 检查custom目录是否为空
    DIR *dir = opendir("custom");
    if (dir) {
        struct dirent *entry;
        bool has_files = false;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {  // 跳过 . 和 .. 目录
                has_files = true;
                break;
            }
        }
        closedir(dir);
        
        if (!has_files) {
            log_info("Custom directory is empty, skipping custom config");
            return true;
        }
    }
    
    // 获取RIME目录
    const char* rime_dir = get_rime_dir();
    if (!rime_dir) {
        log_error("Failed to get RIME directory");
        return false;
    }
    
    // 构建复制命令
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "cp -f custom/* %s/ 2>/dev/null || true", rime_dir);
    
    // 执行复制命令
    if (execute_command(cmd) != 0) {
        log_error("Failed to copy custom configuration");
        return false;
    }
    
    log_info("Custom configuration applied successfully");
    return true;
}

// 自动模式
deploy_status_t deploy_auto(void) {
    log_info("Starting auto deployment...");
    
    if (!install_rime()) return DEPLOY_ERROR_INSTALL;
    if (!backup_config()) return DEPLOY_ERROR_BACKUP;
    if (!install_rime_ice()) return DEPLOY_ERROR_CONFIG;
    if (!apply_custom_config()) return DEPLOY_ERROR_CUSTOM;
    
    log_info("Auto deployment completed successfully");
    return DEPLOY_SUCCESS;
}

// 手动模式
deploy_status_t deploy_manual(deploy_step_t step) {
    log_info("Executing manual step: %d", step);
    
    switch(step) {
        case STEP_INSTALL:
            return install_rime() ? DEPLOY_SUCCESS : DEPLOY_ERROR_INSTALL;
        case STEP_BACKUP:
            return backup_config() ? DEPLOY_SUCCESS : DEPLOY_ERROR_BACKUP;
        case STEP_RIME_ICE:
            return install_rime_ice() ? DEPLOY_SUCCESS : DEPLOY_ERROR_CONFIG;
        case STEP_CUSTOM:
            return apply_custom_config() ? DEPLOY_SUCCESS : DEPLOY_ERROR_CUSTOM;
        default:
            log_error("Invalid step: %d", step);
            return DEPLOY_ERROR_CONFIG;
    }
}

// 升级模式
deploy_status_t deploy_upgrade(bool update_script, bool update_config) {
    if (update_script && !update_deploy_script()) {
        return DEPLOY_ERROR_CONFIG;
    }
    
    if (update_config && !update_rime_config()) {
        return DEPLOY_ERROR_CONFIG;
    }
    
    return DEPLOY_SUCCESS;
}

// 实现更新部署脚本的函数
bool update_deploy_script(void) {
    log_info("Checking for script updates...");
    
    // 克隆或更新仓库到临时目录
    const char* temp_dir = "/tmp/rime-ice-deploy";
    if (dir_exists(temp_dir)) {
        execute_command("rm -rf /tmp/rime-ice-deploy");
    }
    
    if (execute_command("git clone --depth=1 https://github.com/your-repo/rime-ice-deploy.git /tmp/rime-ice-deploy") != 0) {
        log_error("Failed to download latest deploy script");
        return false;
    }
    
    // 复制新版本脚本到当前位置
    if (execute_command("cp -f /tmp/rime-ice-deploy/deploy . && chmod +x deploy") != 0) {
        log_error("Failed to update deploy script");
        return false;
    }
    
    // 清理临时文件
    execute_command("rm -rf /tmp/rime-ice-deploy");
    
    log_info("Deploy script updated successfully");
    return true;
}

// 实现更新RIME配置的函数
bool update_rime_config(void) {
    log_info("Updating RIME configuration...");
    
    const char* rime_dir = get_rime_dir();
    if (!rime_dir) {
        log_error("Failed to get RIME directory");
        return false;
    }
    
    // 检查custom目录是否存在
    if (!dir_exists("custom")) {
        log_info("Custom directory does not exist, skipping custom config");
        return true;
    }
    
    // 备份现有配置
    if (!backup_config()) {
        return false;
    }
    
    // 更新配置文件
    if (!install_rime_ice()) {
        return false;
    }
    
    // 应用自定义配置
    if (!apply_custom_config()) {
        return false;
    }
    
    log_info("RIME configuration updated successfully");
    return true;
}

