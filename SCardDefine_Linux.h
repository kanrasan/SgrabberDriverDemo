#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>



#define CARD_FILE "/dev/xdma0_user"
#define BAR0_SIZE (8 * 1024 * 1024)  // 8MB
#define XDMA_CAMERA_COUNT (4)
#define BufferSize 6400

#define XDMA_IOC_MAGIC 'x'

//==============================User IOCTL=========================
typedef struct user_event0_status {
    uint32_t index;
    uint32_t status;
}USER_EVENT0_STATUS;

#define IOCTL_XDMA_GET_USER_EVENT0_STATUS _IOWR(XDMA_IOC_MAGIC, 0x1A, struct user_event0_status)


//==============================Camera IOCTL======================
typedef struct xdma_start_info
{
    // void *semListFull;
    // void *semListEmpty;
    uint32_t semCount;
} XDMA_START_INFO, *PXDMA_START_INFO;

typedef struct xdma_buffer_info
{
    uint32_t  index;
    uint32_t  size;
} XDMA_BUFFER_INFO, *PXDMA_BUFFER_INFO;

typedef struct xdma_current_frame_index
{
    uint32_t frameIndex;
} XDMA_CURRENT_FRAME_INDEX, *PXDMA_CURRENT_FRAME_INDEX;

//@ release single buffer used
typedef struct xdma_desc_info
{
    uint32_t index;
} XDMA_DESC_INFO, *PXDMA_DESC_INFO;


// // xdma enabled
#define IOCTL_XDMA_CAPTURE_START _IOW('q',2,struct xdma_start_info *)
// xdma disabled
#define IOCTL_XDMA_CAPTURE_STOP   _IO('q', 3)
// alloc frame buffer
#define IOCTL_XDMA_BUFFER_ALLOCK  _IOWR('q', 4, struct xdma_buffer_info *)
// release all frame buffer
#define IOCTL_XDMA_BUFFER_RELEASE _IO('q', 5)
// release buffer at last of buffer list
#define IOCTL_XDMA_BUFFER_RELEASE_LAST _IOW('q', 6, struct xdma_desc_info *)

// get current frame index
#define IOCTL_XDMA_GET_CURRENT_FRAME_INDEX     _IOR('q', 10, struct xdma_current_frame_index *)
// release current frame index
#define IOCTL_XDMA_RELEASE_CURRENT_FRAME_INDEX _IOW('q', 11, struct xdma_current_frame_index *)


