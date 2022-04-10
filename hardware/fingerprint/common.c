#include "common.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#if PLATFORM_SDK_VERSION >= 28
#include <bits/epoll_event.h>
#endif
#include <sys/epoll.h>

#define LOG_TAG "FPC COMMON"

#include <log/log.h>

#define EVENT_COUNT 2

err_t fpc_event_create(fpc_event_t *event, int event_fd) {
    int fd = 0, rc;

    event->event_fd = event_fd;

    fd = open("/dev/fingerprint", O_RDWR);
    if (fd < 0) {
        ALOGE("Error opening FPC device");
        return -1;
    }
    event->dev_fd = fd;

    fd = epoll_create1(0);
    if (fd < 0) {
        ALOGE("Error creating epoll fd");
        return -1;
    }
    event->epoll_fd = fd;

    {
        struct epoll_event ev = {
            .data.fd = event_fd,
            .events = EPOLLIN,
        };
        rc = epoll_ctl(event->epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
        if (rc) {
            ALOGE("Failed to add event_fd to epoll: %d", rc);
            return -1;
        }
    }
    {
        struct epoll_event ev = {
            .data.fd = event->dev_fd,
            .events = EPOLLIN,
        };
        rc = epoll_ctl(event->epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
        if (rc) {
            ALOGE("Failed to add event->dev_fd to epoll: %d", rc);
            return -1;
        }
    }

    return 0;
}

err_t fpc_event_destroy(fpc_event_t *event) {
    event->event_fd = -1;
    close(event->dev_fd);
    event->dev_fd = -1;
    close(event->epoll_fd);
    event->epoll_fd = -1;
    return 0;
}

err_t fpc_set_power(const fpc_event_t *event, int poweron)
{
    int ret = -1;

    ret = ioctl(event->dev_fd, FPC_IOCWPREPARE, poweron);
    if (ret < 0) {
        ALOGE("Error preparing FPC device\n");
        return -1;
    }

    return 1;
}

err_t fpc_get_power(const fpc_event_t *event)
{
    int ret = -1;
    uint32_t reply = -1;

    ret = ioctl(event->dev_fd, FPC_IOCRPREPARE, &reply);
    if (ret < 0) {
        ALOGE("Error preparing FPC device\n");
        return -1;
    }

    if (reply > 1)
        return -1;

    return reply;
}

err_t fpc_poll_event(const fpc_event_t *event)
{
    int cnt;

    struct epoll_event events[EVENT_COUNT];
    cnt = epoll_wait(event->epoll_fd, events, EVENT_COUNT, -1);

    if (cnt < 0)
    {
        ALOGE("Failed waiting for epoll: %d", cnt);
        return FPC_EVENT_ERROR;
    }

    if (!cnt) {
        ALOGE("Epoll timed out despite infinite blocking!");
        return FPC_EVENT_TIMEOUT;
    }

    for (int i = 0; i < cnt; ++i)
        if (events[i].data.fd == event->event_fd && events[i].events | EPOLLIN)
        {
            ALOGD("Waking up from eventfd");
            return FPC_EVENT_EVENTFD;
        }

    // Only other event source is the fingerprint.
    ALOGD("Waking up from finger event");
    return FPC_EVENT_FINGER;
}

err_t fpc_keep_awake(const fpc_event_t *event, int awake, unsigned int timeout) {
    struct {
        int awake;
        unsigned int timeout;
    } args = {awake, timeout};
    int rc = ioctl(event->dev_fd, FPC_IOCWAWAKE, &args);
    if (rc)
        ALOGE("%s failed: %d", __func__, rc);
    return rc;
}
