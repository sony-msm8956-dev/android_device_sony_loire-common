#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOG_TAG "macaddrsetup"
#include <cutils/log.h>

#define LIB_TA "libta.so"

#define BT_MAC_FILE "/data/vendor/bluetooth/bluetooth_bdaddr"

extern const char *__progname;

int main(int argc, char **argv)
{
        uint32_t size;
        char buf[6];
        FILE *fpb, *fpw = NULL;
        mode_t orig_mask;
        int ret, err, bt_addr, wl_addr;
        void *ta_handle = NULL;
        int (*ta_open)(uint8_t p, uint8_t m, uint8_t c) = NULL;
        int (*ta_close)(void) = NULL;
        int (*ta_getsize)(uint32_t id, uint32_t *size) = NULL;
        int (*ta_read)(uint32_t id, void *buf, uint32_t size) = NULL;

#ifdef BRCMFMAC
        struct ifreq ifr;
        int sockfd;
#endif

        // Sony had a check for ro.hardware here, but since all supported devices were added here anyways,
        // and the values are the same, it has been removed.
        wl_addr = 2560;
        bt_addr = 2568;

        ta_handle = dlopen(LIB_TA, RTLD_NOW);
        if (!ta_handle) {
                ALOGE("%s: DLOPEN failed for %s", __func__, LIB_TA);
                exit(1);
        }

        ta_open = dlsym(ta_handle, "ta_open");
        if (!ta_open) {
                ALOGE("%s: DLSYM failed for ta_open", __func__);
                exit(1);
        }

        ta_close = dlsym(ta_handle, "ta_close");
        if (!ta_close) {
                ALOGE("%s: DLSYM failed for ta_close", __func__);
                exit(1);
        }

        ta_getsize = dlsym(ta_handle, "ta_getsize");
        if (!ta_open) {
                ALOGE("%s: DLSYM failed for ta_getsize", __func__);
                exit(1);
        }

        ta_read = dlsym(ta_handle, "ta_read");
        if (!ta_read) {
                ALOGE("%s: DLSYM failed for ta_read", __func__);
                exit(1);
        }

        for (;;) {
                err = ta_open(2, 0x1, 1);
                if (!err)
                        break;

                SLOGE("failed to open misc ta: %d\n", err);
                sleep(5);
        }

        // This file needs to be readable by the bluetooth group
        orig_mask = umask(0026);
        fpb = fopen(BT_MAC_FILE, "w");
        umask(orig_mask);
        if (!fpb) {
                SLOGE("failed to open %s for writing\n", BT_MAC_FILE);
                ta_close();
                exit(1);
        }

        err = ta_getsize(bt_addr, &size);
        if (size != 6) {
                SLOGE("BT mac address have wrong size (%d) in miscta", size);
                ta_close();
                fclose(fpb);
                exit(1);
        }

        err = ta_read(bt_addr, buf, size);
        if (err) {
                SLOGE("failed to read BT mac address from miscta");
                ta_close();
                fclose(fpb);
                exit(1);
        }

#ifdef MIRROR_MAC_ADDRESS
        ret = fprintf(fpb, "%02x:%02x:%02x:%02x:%02x:%02x\n",
                      buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
#else
        ret = fprintf(fpb, "%02x:%02x:%02x:%02x:%02x:%02x\n",
                      buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
#endif
        if (ret != 18) {
                SLOGE("failed to write BT mac address\n");
                ta_close();
                fclose(fpb);
                exit(1);
        }

        if (argc > 1) {
                err = ta_getsize(wl_addr, &size);
                if (size != 6) {
                        SLOGE("mac address have wrong size (%d) in miscta", size);
                        ta_close();
                        fclose(fpw);
                        exit(1);
                }

                err = ta_read(wl_addr, buf, size);
                if (err) {
                        SLOGE("failed to read mac address from miscta");
                        ta_close();
                        fclose(fpw);
                        exit(1);
                }
#ifdef BRCMFMAC
                sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                if (sockfd < 0) {
                        SLOGE("failed to open socket\n");
                        ta_close();
                        exit(1);
                }
                strcpy(ifr.ifr_name, "wlan0");
                ifr.ifr_hwaddr.sa_data[0] = buf[5];
                ifr.ifr_hwaddr.sa_data[1] = buf[4];
                ifr.ifr_hwaddr.sa_data[2] = buf[3];
                ifr.ifr_hwaddr.sa_data[3] = buf[2];
                ifr.ifr_hwaddr.sa_data[4] = buf[1];
                ifr.ifr_hwaddr.sa_data[5] = buf[0];
                ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
                ret = ioctl(sockfd, SIOCSIFHWADDR, &ifr);
                if (ret != 0) {
                        SLOGE("failed to write WLAN mac address using ioctl %d\n",
                              ret);
                        ta_close();
                        close(sockfd);
                        exit(1);
                }
                close(sockfd);
#else
                fpw = fopen(argv[1], "w");
                if (!fpw) {
                        SLOGE("failed to open %s for writing: %s\n",
                              argv[1], strerror(errno));
                        ta_close();
                        exit(1);
                }
#ifdef QCA_CLD3_WIFI
#ifdef MIRROR_MAC_ADDRESS
                ret = fprintf(fpw, "Intf0MacAddress=%02X%02X%02X%02X%02X%02X\nEND\n",
                              buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
#else
                ret = fprintf(fpw, "Intf0MacAddress=%02X%02X%02X%02X%02X%02X\nEND\n",
                              buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
#endif
                if (ret != 33) {
#else
                ret = fprintf(fpw, "%02x:%02x:%02x:%02x:%02x:%02x\n",
                              buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
                if (ret != 18) {
#endif
                        SLOGE("failed to write WLAN mac address\n");
                        ta_close();
                        fclose(fpw);
                        exit(1);
                }
#endif
        }
        ta_close();
        fclose(fpb);
        if (fpw)
                fclose(fpw);
        dlclose(ta_handle);

        return 0;
}
