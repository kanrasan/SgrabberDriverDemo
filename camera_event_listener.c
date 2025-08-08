#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main() {
    int cameraCount = 4; // 设备数量
    struct pollfd fds[16]; // 最多16个
    char devName[128];

    // 打开设备文件
    for (int i = 0; i < cameraCount; i++) {
        snprintf(devName, sizeof(devName),
                 "/dev/xdma0_cameraStatusEvents_%d", i);
        fds[i].fd = open(devName, O_RDONLY);
        if (fds[i].fd < 0) {
            perror(devName);
            return 1;
        }
        fds[i].events = POLLIN;
    }

    // 把标准输入也加入poll监听
    fds[cameraCount].fd = STDIN_FILENO; // 0
    fds[cameraCount].events = POLLIN;

    printf("开始监控 %d 个设备...\n", cameraCount);

    while (1) {
        printf("是否退出？（请输入Y）\n");

        int ret = poll(fds, cameraCount + 1, -1); // 加1监听stdin
        if (ret < 0) {
            perror("poll");
            break;
        }

        // 检查设备事件
        for (int i = 0; i < cameraCount; i++) {
            if (fds[i].revents & POLLIN) {
                printf("设备 %d 有事件！\n", i);

                char buf[64];
                int n = read(fds[i].fd, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("设备 %d 数据: %s\n", i, buf);
                }
            }
            if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                printf("设备 %d 出错或断开！\n", i);
            }
        }

        // 检查标准输入
        if (fds[cameraCount].revents & POLLIN) {
            char input[16];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                // 去除换行符
                input[strcspn(input, "\r\n")] = 0;
                if (strlen(input) == 1 && (input[0] == 'Y' || input[0] == 'y')) {
                    printf("收到退出指令，退出程序。\n");
                    goto cleanup;
                } else {
                    printf("未退出，继续监控...\n");
                }
            }
        }
    }

cleanup:
    for (int i = 0; i < cameraCount; i++) {
        close(fds[i].fd);
    }
    return 0;
}
