LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    macaddrsetup.c

ifeq ($(WIFI_DRIVER_BUILT),qca_cld3)
LOCAL_CFLAGS += -DQCA_CLD3_WIFI
endif

ifeq ($(BOARD_HAS_MIRROR_MACADDRESS),true)
LOCAL_CFLAGS += -DMIRROR_MAC_ADDRESS
endif

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils

LOCAL_MODULE := macaddrsetup
ifneq ($(call math_gt_or_eq, $(PLATFORM_SDK_VERSION), 25),)
LOCAL_MODULE_OWNER := sony
LOCAL_INIT_RC_64   := macaddrsetup.rc
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
