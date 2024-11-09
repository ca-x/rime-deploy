#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// 配置结构体
typedef struct {
    char rime_dir[512];        // RIME安装目录
    char backup_dir[512];      // 备份目录
    bool backup_enabled;       // 是否启用备份
    bool auto_deploy;          // 是否自动部署
    bool custom_config;        // 是否使用自定义配置
    struct {
        bool enabled;          // 是否启用模糊音
        bool ch_zh;           // zh、ch
        bool sh_s;            // sh、s
        bool eng_ing;         // eng、ing
        bool an_ang;          // an、ang
    } fuzzy;                  // 模糊音配置
} config_t;

// 配置相关函数
config_t* config_create(void);
void config_free(config_t *config);
bool config_setup_interactive(config_t *config);
bool config_validate(const config_t *config);

#endif // CONFIG_H