#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

int main() {
    int fd = open("/dev/xdma0_cameraStatusEvents_0", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLIN;

    while (1) {
        int ret = poll(&fds, 1, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }
        if (fds.revents & POLLIN) {
            char buf[128];
            int n = read(fd, buf, sizeof(buf));
            if (n > 0) {
                printf("Got event: %.*s\n", n, buf);
            }
        }
    }

    close(fd);
    return 0;
}
