#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cutils/properties.h>
#include <private/android_filesystem_config.h>

#define LOG_TAG "macaddrsetup"
#include <cutils/log.h>

#define LIB_TA "libta.so"

#define BT_MAC_FILE "/data/misc/bluetooth/bluetooth_bdaddr"

extern const char *__progname;

int main(int argc, char **argv)
{
	uint32_t size;
	char buf[6];
	FILE *fpb, *fpw = NULL;
	int ret, err, bt_addr, wl_addr;
	void *ta_handle = NULL;
	int (*ta_open)(uint8_t p, uint8_t m, uint8_t c) = NULL;
	int (*ta_close)(void) = NULL;
	int (*ta_getsize)(uint32_t id, uint32_t *size) = NULL;
	int (*ta_read)(uint32_t id, void *buf, uint32_t size) = NULL;

	// Sony had a check for ro.hardware here, but since all supported devices were added here anyways,
	// and the values are the same, it has been removed.
	wl_addr=2560;
	bt_addr=2568;

	ta_handle = dlopen(LIB_TA, RTLD_NOW);
	if (!ta_handle) {
		ALOGE("%s: DLOPEN failed for %s", __func__, LIB_TA);
	} else {
		ta_open = dlsym(ta_handle, "ta_open");
		ta_close = dlsym(ta_handle, "ta_close");
		ta_getsize = dlsym(ta_handle, "ta_getsize");
		ta_read = dlsym(ta_handle, "ta_read");
	}

	for (;;) {
		err = ta_open(2, 0x1, 1);
		if (!err)
			break;

		SLOGE("failed to open misc ta: %d\n",err);
		sleep(5);
	}

	fpb = fopen(BT_MAC_FILE, "w");
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

	ret = fprintf(fpb, "%02x:%02x:%02x:%02x:%02x:%02x\n", buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
	if (ret != 18) {
		SLOGE("failed to write BT mac address\n");
		ta_close();
		fclose(fpb);
		exit(1);
	}

	if (argc > 1) {
		fpw = fopen(argv[1], "w");
		if (!fpw) {
			SLOGE("failed to open %s for writing: %s\n", argv[1], strerror(errno));
			ta_close();
			exit(1);
		}

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

		ret = fprintf(fpw, "%02x:%02x:%02x:%02x:%02x:%02x\n", buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
		if (ret != 18) {
			SLOGE("failed to write WLAN mac address\n");
			ta_close();
			fclose(fpw);
			exit(1);
		}
	}
	ta_close();
	fclose(fpb);
	if (fpw)
		fclose(fpw);
	dlclose(ta_handle);

	return 0;
}
