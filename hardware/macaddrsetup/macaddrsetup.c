#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cutils/properties.h>

#define LOG_TAG "macaddrsetup"
#include <cutils/log.h>

extern const char *__progname;
extern int ta_open(uint8_t p, uint8_t m, uint8_t c);
extern int ta_close(void);
extern int ta_getsize(uint32_t id, uint32_t *size);
extern int ta_read(uint32_t id, void *buf, uint32_t size);

int main(int argc, char **argv)
{
	uint32_t size;
	char buf[6];
	FILE *fpb, *fpw = NULL;
	char record[PROPERTY_VALUE_MAX];
	int ret, err, bt_addr, wl_addr;

	property_get("ro.hardware",record,"");
	SLOGI("Importing BT and WLAN address for %s device\n", record);

	if ((strcmp(record,"seagull")==0)||(strcmp(record,"tianchi")==0)){
		wl_addr=2560;
		bt_addr=2568;
	} else if ((strcmp(record,"amami")==0)||(strcmp(record,"honami")==0)||(strcmp(record,"togari")==0)){
		wl_addr=2560;
		bt_addr=2568;
	} else if ((strcmp(record,"sirius")==0)||(strcmp(record,"castor")==0)||(strcmp(record,"castor_windy")==0)||(strcmp(record,"leo")==0)
			||(strcmp(record,"aries")==0)||(strcmp(record,"scorpion")==0)||(strcmp(record,"scorpion_windy")==0)){
		wl_addr=2560;
		bt_addr=2568;
	} else if ((strcmp(record,"tulip")==0)){
		wl_addr=2560;
		bt_addr=2568;
	} else if ((strcmp(record,"ivy")==0)||(strcmp(record,"karin")==0)||(strcmp(record,"karin_windy")==0)||(strcmp(record,"sumire")==0)
			||(strcmp(record,"suzuran")==0)||(strcmp(record,"satsuki")==0)){
		wl_addr=2560;
		bt_addr=2568;
        } else if ((strcmp(record,"suzu")==0)){
		wl_addr=2560;
		bt_addr=2568;
	} else {
		SLOGE("Unsupported device\n");
		exit(1);
	}

	for (;;) {
		err = ta_open(2, 0x1, 1);
		if (!err)
			break;

		SLOGE("failed to open misc ta: %d\n",err);
		sleep(5);
	}

	fpb = fopen("/data/etc/bluetooth_bdaddr", "w");
	if (!fpb) {
		SLOGE("failed to open /data/etc/bluetooth_bdaddr for writing:\n");
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

	return 0;
}
