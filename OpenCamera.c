#include "SCardDefine_Linux.h"


int main() {
    int fd_user = open(CARD_FILE, O_RDWR | O_SYNC);
    if (fd_user < 0) {
        printf("open card failed");
        return -1;
    }
    printf("card opened\n");

    int fds_camera[4]; 
    char devName[128];
        // 打开设备文件
    for (int i = 0; i < XDMA_CAMERA_COUNT; i++) 
    {
        snprintf(devName, sizeof(devName),
                 "/dev/xdma0_camera_%d", i);
        fds_camera[i] = open(devName, O_RDWR | O_SYNC);
        if(fds_camera[i]<0)
        {
            printf("open camera failed");
            goto cleanup_fd_user;
        }
    }

    void *bar0_addr = mmap(NULL, BAR0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_user, 0);
    if (bar0_addr == MAP_FAILED) {
        printf("mmap failed");
        goto cleanup_fds_camera;
    }
    printf("BAR0 mapped at %p\n", bar0_addr);

    //volatile unsigned int *regs = (volatile unsigned int *)bar0_addr;
    uint32_t  bufferCount=0;
    while (1) {
        printf("是否分配一块内存？总内存数%u(请输入 Y/N) :",bufferCount);
        int c = getchar();

        // 读掉可能的多余换行符
        while (getchar() != '\n');

        c = tolower(c);
        if (c == 'y') {
             
             struct xdma_buffer_info arg;
             arg.index=bufferCount;
             arg.size=BufferSize;
            if (ioctl(fds_camera[0], IOCTL_XDMA_BUFFER_ALLOCK, &arg) < 0) {
                printf("ioctl分配内存(%u)失败\n",bufferCount);
            }
            else
            {
                printf("ioctl分配内存(%u)成功\n", bufferCount);
                bufferCount++;
            }
        } else if (c == 'n') {
            printf("退出程序，清理资源。\n");
            break;
        } else {
            printf("无效输入，请输入 Y 或 N。\n");
        }
    }

cleanup_fds_camera:

    for(int i=bufferCount-1;i>-1;i--)
    {
        XDMA_DESC_INFO arg;
        arg.index=i;
        if (ioctl(fds_camera[0], IOCTL_XDMA_BUFFER_RELEASE_LAST, &arg) < 0) {
            printf("ioctl释放内存（%d）失败\n",i);
        }
        else
        {
            printf("ioctl释放内存（%d）成功\n", i);
            bufferCount--;
        }
    }
    printf("剩余内存数量：%u",bufferCount);
    for (int i = 0; i < XDMA_CAMERA_COUNT; i++) 
    {
        close(fds_camera[i]);
        printf("关闭相机%u",i);
    }


cleanup_fd_user:
    close(fd_user);
    printf("关闭采集卡");
    return 0;
}
