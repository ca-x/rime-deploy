#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"  // 添加config.h头文件
#include "deploy.h"
#include "platform.h"
#include "utils.h"

void print_usage(void) {
    printf("Usage: rime-deploy [options]\n\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n");
}

void print_version(void) {
    printf("rime-deploy version 1.0.1\n");
}

int main(int argc, char *argv[]) {
    // 初始化日志
    if (!init_logging()) {
        fprintf(stderr, "Failed to initialize logging\n");
        return 1;
    }

    // 处理命令行参数
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage();
            cleanup_logging();
            return 0;
        }
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            print_version();
            cleanup_logging();
            return 0;
        }
    }

    // 创建配置
    config_t *config = config_create();
    if (!config) {
        log_error("Failed to create config");
        cleanup_logging();
        return 1;
    }

    // 用户交互配置
    if (!config_setup_interactive(config)) {
        log_error("Failed to setup config");
        config_free(config);
        cleanup_logging();
        return 1;
    }

    // 执行部署
    deploy_status_t status = deploy_auto();
    if (status != DEPLOY_SUCCESS) {
        log_error("Deployment failed with status: %d", status);
        config_free(config);
        cleanup_logging();
        return 1;
    }

    // 清理资源
    config_free(config);
    cleanup_logging();
    log_info("Deployment completed successfully");

    return 0;
}