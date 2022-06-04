LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    macaddrsetup.c

ifeq ($(WIFI_DRIVER_BUILT),qca_cld3)
LOCAL_CFLAGS += -DQCA_CLD3_WIFI
endif

ifeq ($(WIFI_DRIVER_BUILT),brcmfmac)
LOCAL_CFLAGS += -DBRCMFMAC
endif

ifeq ($(BOARD_HAS_MIRROR_MACADDRESS),true)
LOCAL_CFLAGS += -DMIRROR_MAC_ADDRESS
endif

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils

LOCAL_MODULE := macaddrsetup
LOCAL_MODULE_OWNER := sony
LOCAL_INIT_RC      := macaddrsetup.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
