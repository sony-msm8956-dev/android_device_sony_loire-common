MM_V4L2_DRIVER_LIST += msm8960
MM_V4L2_DRIVER_LIST += msm8974
MM_V4L2_DRIVER_LIST += msm8226
MM_V4L2_DRIVER_LIST += msm8610
MM_V4L2_DRIVER_LIST += apq8084
MM_V4L2_DRIVER_LIST += mpq8092
MM_V4L2_DRIVER_LIST += msm_bronze
MM_V4L2_DRIVER_LIST += msm8916
MM_V4L2_DRIVER_LIST += msm8994
MM_V4L2_DRIVER_LIST += msm8084
MM_V4L2_DRIVER_LIST += msm8909
MM_V4L2_DRIVER_LIST += msm8952
MM_V4L2_DRIVER_LIST += msm8996
MM_V4L2_DRIVER_LIST += msm8992
MM_V4L2_DRIVER_LIST += msm8937
MM_V4L2_DRIVER_LIST += msm8953
MM_V4L2_DRIVER_LIST += apq8098_latv
MM_V4L2_DRIVER_LIST += msm8998
MM_V4L2_DRIVER_LIST += sdm660

display-hal := $(COMMON_PATH)/hardware/qcom/display/msm8998
QCOM_MEDIA_ROOT := $(COMMON_PATH)/hardware/qcom/media/msm8998
OMX_VIDEO_PATH := mm-video-v4l2
media-hal := $(QCOM_MEDIA_ROOT)
SRC_DISPLAY_HAL_DIR := $(display-hal)
SRC_MEDIA_HAL_DIR := $(QCOM_MEDIA_ROOT)

ifneq (,$(filter $(MM_V4L2_DRIVER_LIST),$(TARGET_BOARD_PLATFORM)))
  ifneq ($(strip $(USE_CAMERA_STUB)),true)
    ifneq ($(BUILD_TINY_ANDROID),true)
      include $(call all-subdir-makefiles)
    endif
  endif
endif

include $(display-hal)/Android.mk
include $(call all-makefiles-under,$(media-hal))
