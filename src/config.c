#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "platform.h"
#include "utils.h"

// 创建默认配置
config_t* config_create(void) {
    config_t *config = calloc(1, sizeof(config_t));
    if (!config) {
        log_error("Failed to allocate memory for config");
        return NULL;
    }

    // 设置默认值
    const char* rime_dir = get_rime_dir();
    if (rime_dir) {
        strncpy(config->rime_dir, rime_dir, sizeof(config->rime_dir) - 1);
    }

    const char* backup_dir = get_default_backup_dir();
    if (backup_dir) {
        strncpy(config->backup_dir, backup_dir, sizeof(config->backup_dir) - 1);
    }

    // 默认配置
    config->backup_enabled = true;
    config->auto_deploy = true;
    config->custom_config = false;

    // 默认模糊音配置
    config->fuzzy.enabled = false;
    config->fuzzy.ch_zh = false;
    config->fuzzy.sh_s = false;
    config->fuzzy.eng_ing = false;
    config->fuzzy.an_ang = false;

    return config;
}

// 释放配置
void config_free(config_t *config) {
    if (config) {
        free(config);
    }
}

// 用户交互配置
bool config_setup_interactive(config_t *config) {
    char input[8];

    printf("\n=== RIME Deploy Configuration ===\n");

    // 询问部署模式
    printf("\nDo you want to use auto deploy mode? [Y/n]: ");
    fgets(input, sizeof(input), stdin);
    config->auto_deploy = (input[0] == '\n' || input[0] == 'y' || input[0] == 'Y');

    // 询问是否启用备份
    printf("\nEnable configuration backup? [Y/n]: ");
    fgets(input, sizeof(input), stdin);
    config->backup_enabled = (input[0] == '\n' || input[0] == 'y' || input[0] == 'Y');

    // 询问是否使用自定义配置
    printf("\nUse custom configuration? [y/N]: ");
    fgets(input, sizeof(input), stdin);
    config->custom_config = (input[0] == 'y' || input[0] == 'Y');

    // 如果使用自定义配置，询问模糊音设置
    if (config->custom_config) {
        printf("\n=== Fuzzy Pinyin Configuration ===\n");

        printf("Enable fuzzy pinyin? [y/N]: ");
        fgets(input, sizeof(input), stdin);
        config->fuzzy.enabled = (input[0] == 'y' || input[0] == 'Y');

        if (config->fuzzy.enabled) {
            printf("Enable zh<->ch? [y/N]: ");
            fgets(input, sizeof(input), stdin);
            config->fuzzy.ch_zh = (input[0] == 'y' || input[0] == 'Y');

            printf("Enable sh<->s? [y/N]: ");
            fgets(input, sizeof(input), stdin);
            config->fuzzy.sh_s = (input[0] == 'y' || input[0] == 'Y');

            printf("Enable eng<->ing? [y/N]: ");
            fgets(input, sizeof(input), stdin);
            config->fuzzy.eng_ing = (input[0] == 'y' || input[0] == 'Y');

            printf("Enable an<->ang? [y/N]: ");
            fgets(input, sizeof(input), stdin);
            config->fuzzy.an_ang = (input[0] == 'y' || input[0] == 'Y');
        }
    }

    printf("\nConfiguration completed!\n");
    return true;
}

// 验证配置
bool config_validate(const config_t *config) {
    if (!config) return false;

    // 验证RIME目录
    if (strlen(config->rime_dir) == 0) {
        log_error("RIME directory is not set");
        return false;
    }

    // 如果启用备份,验证备份目录
    if (config->backup_enabled && strlen(config->backup_dir) == 0) {
        log_error("Backup directory is not set");
        return false;
    }

    return true;
}