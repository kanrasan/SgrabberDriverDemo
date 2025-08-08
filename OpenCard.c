
#include "SCardDefine_Linux.h"


int main() {
    int fd_user = open(CARD_FILE, O_RDWR | O_SYNC);
    if (fd_user < 0) {
        perror("open card failed");
        return -1;
    }
    printf("card opened\n");

    void *bar0_addr = mmap(NULL, BAR0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_user, 0);
    if (bar0_addr == MAP_FAILED) {
        perror("mmap failed");
        close(fd_user);
        return -1;
    }
    printf("BAR0 mapped at %p\n", bar0_addr);

    volatile unsigned int *regs = (volatile unsigned int *)bar0_addr;

    while (1) {
        printf("是否读取寄存器 0xC84C 的值？请输入 Y/N ：");
        int c = getchar();

        // 读掉可能的多余换行符
        while (getchar() != '\n');

        c = tolower(c);
        if (c == 'y') {
            // 0xC84C 是字节偏移，转换成寄存器索引（4字节宽）
            unsigned int offset = 0xC84C / 4;
            unsigned int val = regs[offset];
            printf("寄存器 0xC84C 的值为：0x%08x\n", val);
        } else if (c == 'n') {
            printf("退出程序，清理资源。\n");
            break;
        } else {
            printf("无效输入，请输入 Y 或 N。\n");
        }
    }

    munmap(bar0_addr, BAR0_SIZE);
    close(fd_user);
    return 0;
}
