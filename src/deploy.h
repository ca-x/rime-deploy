#ifndef DEPLOY_H
#define DEPLOY_H

#include <stdbool.h>
#include "config.h"

// 部署状态枚举
typedef enum {
    DEPLOY_SUCCESS = 0,
    DEPLOY_ERROR_INSTALL,
    DEPLOY_ERROR_BACKUP,
    DEPLOY_ERROR_CONFIG,
    DEPLOY_ERROR_CUSTOM
} deploy_status_t;

// 部署模式枚举
typedef enum {
    MODE_AUTO,
    MODE_MANUAL,
    MODE_UPGRADE
} deploy_mode_t;

// 部署步骤枚举
typedef enum {
    STEP_INSTALL = 1,
    STEP_BACKUP = 2,
    STEP_RIME_ICE = 3,
    STEP_CUSTOM = 4
} deploy_step_t;

// 部署函数
deploy_status_t deploy_auto(void);
deploy_status_t deploy_manual(deploy_step_t step);
deploy_status_t deploy_upgrade(bool update_script, bool update_config);


// 具体步骤函数
bool install_rime(void);
bool backup_config(void);
bool install_rime_ice(void);
bool apply_custom_config(void);

// 升级相关函数
bool update_deploy_script(void);
bool update_rime_config(void);

#endif // DEPLOY_H