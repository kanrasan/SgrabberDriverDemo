#include "SCardDefine_Linux.h"


int main() {
    int fd_user = open(CARD_FILE, O_RDWR | O_SYNC);
    if (fd_user < 0) {
        perror("open device failed");
        return -1;
    }
    printf("device opened\n");

    struct pollfd fds_cameraStatus[5]; 
    char devName[128];

    // 打开设备文件
    for (int i = 0; i < XDMA_CAMERA_COUNT; i++) {
        snprintf(devName, sizeof(devName),
                 "/dev/xdma0_cameraStatusEvents_%d", i);
        fds_cameraStatus[i].fd = open(devName, O_RDONLY);
        if (fds_cameraStatus[i].fd < 0) {
            perror(devName);
            goto cleanup_fd_user;
        }
        fds_cameraStatus[i].events = POLLIN;
    }

    // 把标准输入也加入poll监听
    fds_cameraStatus[XDMA_CAMERA_COUNT].fd = STDIN_FILENO; // 0
    fds_cameraStatus[XDMA_CAMERA_COUNT].events = POLLIN;

    printf("开始监控 %d 个设备...\n", XDMA_CAMERA_COUNT);

    while (1) {
        printf("是否退出?(请输入Y)\n");

        int ret = poll(fds_cameraStatus, XDMA_CAMERA_COUNT + 1, -1); // 加1监听stdin
        if (ret < 0) {
            perror("poll");
            break;
        }

        // 检查设备事件
        for (int i = 0; i < XDMA_CAMERA_COUNT; i++) {
            if (fds_cameraStatus[i].revents & POLLIN) {
                printf("设备 %d 有事件！\n", i);
                struct user_event0_status evt;
                evt.index=i;
                evt.status=0;
                if (ioctl(fd_user, IOCTL_XDMA_GET_USER_EVENT0_STATUS, &evt) < 0) {
                    printf("ioctl获取设备状态失败");
                }
                else
                {
                    printf("设备 %d 状态：%u\n", i,evt.status);
                }
            }
            if (fds_cameraStatus[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                printf("设备 %d 出错或断开！\n", i);
            }
        }

        // 检查标准输入
        if (fds_cameraStatus[XDMA_CAMERA_COUNT].revents & POLLIN) {
            char input[16];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                // 去除换行符
                input[strcspn(input, "\r\n")] = 0;
                if (strlen(input) == 1 && (input[0] == 'Y' || input[0] == 'y')) {
                    printf("收到退出指令，退出程序。\n");
                    goto cleanup_fds_cameraStatus;
                } else {
                    printf("未退出，继续监控...\n");
                }
            }
        }
    }

cleanup_fds_cameraStatus:
    for (int i = 0; i < XDMA_CAMERA_COUNT; i++) {
        close(fds_cameraStatus[i].fd);
    }
cleanup_fd_user:
    close(fd_user);
    return 0;
}
