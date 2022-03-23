LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := android.hardware.power@1.2-service.loire
LOCAL_INIT_RC := android.hardware.power@1.2-service.loire.rc
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_HEADER_LIBRARIES := libhardware_headers

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    libdl \
    libhidlbase \
    liblog \
    libutils \
    android.hardware.power@1.2

LOCAL_SRC_FILES := \
    hint-data.c \
    list.c \
    metadata-parser.c \
    power-8952.c \
    Power.cpp \
    power-common.c \
    service.cpp \
    utils.c

LOCAL_CFLAGS += -Wall -Wextra -Werror

ifneq ($(TARGET_TAP_TO_WAKE_NODE),)
    LOCAL_CFLAGS += -DTAP_TO_WAKE_NODE=\"$(TARGET_TAP_TO_WAKE_NODE)\"
endif

ifeq ($(TARGET_USES_INTERACTION_BOOST),true)
    LOCAL_CFLAGS += -DINTERACTION_BOOST
endif

include $(BUILD_EXECUTABLE)
