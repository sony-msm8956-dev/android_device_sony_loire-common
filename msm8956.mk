# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Common path
COMMON_PATH := device/sony/msm8956-common

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(COMMON_PATH)

# Overlay
DEVICE_PACKAGE_OVERLAYS += $(COMMON_PATH)/overlay

PRODUCT_ENFORCE_RRO_TARGETS := \
    framework-res

# Properties
-include $(COMMON_PATH)/system_prop.mk

# APEX
PRODUCT_COMPRESSED_APEX := false

# Audio
PRODUCT_PACKAGES += \
    android.hardware.audio.service \
    android.hardware.audio@6.0-impl \
    android.hardware.audio.effect@6.0-impl \
    android.hardware.audio.effect@2.0-service \
    android.hardware.soundtrigger@2.1-impl \
    audio.primary.msm8952 \
    audio.a2dp.default \
    audio.r_submix.default \
    audio.usb.default \
    libaudioclient_shim \
    libaudio-resampler \
    libtinyalsa \
    libaudioroute \
    tinymix \
    libqcomvisualizer \
    libqcomvoiceprocessing \
    libqcompostprocbundle

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/audio/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
    $(COMMON_PATH)/audio/audio_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info.xml \
    $(COMMON_PATH)/audio/audio_tuning_mixer_tasha.txt:$(TARGET_COPY_OUT_VENDOR)/etc/audio_tuning_mixer_tasha.txt \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml

# Bluetooth
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service \
    libbt-vendor

# Camera
PRODUCT_PACKAGES += \
    android.hardware.camera.provider@2.4-impl \
    android.hardware.camera.provider@2.4-service \
    camera.msm8952 \
    libmmcamera_interface \
    libmmjpeg_interface \
    libmmlib2d_interface \
    libmm-qcamera \
    libqomx_core

# Charger
PRODUCT_PRODUCT_PROPERTIES += \
    ro.charger.enable_suspend=true

# Configstore
PRODUCT_PACKAGES += \
    android.hardware.configstore@1.1-service

# For config.fs
PRODUCT_PACKAGES += \
    fs_config_files \
    fs_config_dirs

# DRM
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service

# Fingerprint
PRODUCT_PACKAGES += \
    android.hardware.biometrics.fingerprint@2.1 \
    android.hardware.biometrics.fingerprint@2.1-service.sony

# Gatekeeper
PRODUCT_PACKAGES += \
    android.hardware.gatekeeper@1.0-impl \
    android.hardware.gatekeeper@1.0-service

# Graphics
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.graphics.mapper@2.0-impl-2.1 \
    copybit.msm8952 \
    gralloc.msm8952 \
    hwcomposer.msm8952 \
    memtrack.msm8952 \
    libqdutils \
    libqdMetaData \
    libLLVM \
    libion

# GPS
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/gps/flp.conf:$(TARGET_COPY_OUT_VENDOR)/etc/flp.conf \
    $(COMMON_PATH)/gps/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps.conf \
    $(COMMON_PATH)/gps/izat.conf:$(TARGET_COPY_OUT_VENDOR)/etc/izat.conf \
    $(COMMON_PATH)/gps/lowi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/lowi.conf \
    $(COMMON_PATH)/gps/sap.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sap.conf \
    $(COMMON_PATH)/gps/xtwifi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/xtwifi.conf

# Health
PRODUCT_PACKAGES += \
    android.hardware.health@2.0-service.sony

# HIDL
PRODUCT_PACKAGES += \
    libhidltransport.vendor \
    libhwbinder.vendor

# Init
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/init/android.hardware.wifi@1.0-service.legacy.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/android.hardware.wifi@1.0-service.legacy.rc

# Keylayout
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/keylayout/clearpad.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/clearpad.idc \
    $(COMMON_PATH)/keylayout/gpio-keys.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/gpio-keys.kl

# Keymaster
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service

# Light
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.sony

# Media
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/media/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(COMMON_PATH)/media/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    $(COMMON_PATH)/media/media_profiles_V1_0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml

# Memtrack
PRODUCT_PACKAGES += \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service

# MISC
PRODUCT_PACKAGES += \
    libqti_vndfwk_detect \
    libqti_vndfwk_detect.vendor \
    libjson \
    libxml2

# MSM IRQ Balancer configuration file
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/msm_irqbalance.conf:$(TARGET_COPY_OUT_VENDOR)/etc/msm_irqbalance.conf

# Netutils
PRODUCT_PACKAGES += \
    netutils-wrapper-1.0

# NFC
PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    NfcNci \
    Tag \
    android.hardware.nfc@1.1-service

# OMX
PRODUCT_PACKAGES += \
    android.hardware.media.omx@1.0-service \
    libc2dcolorconvert \
    libstagefrighthw \
    libOmxCore \
    libmm-omxcore \
    libOmxVdec \
    libOmxVdecHevc \
    libOmxVenc

# OSS Time service
PRODUCT_PACKAGES += \
    timekeep \
    TimeKeep

# OSS WIFI and BT MAC tool
PRODUCT_PACKAGES += \
    macaddrsetup

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.full.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.full.xml \
    frameworks/native/data/etc/android.hardware.camera.raw.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.raw.xml \
    frameworks/native/data/etc/android.hardware.fingerprint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.fingerprint.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute.xml \
    frameworks/native/data/etc/android.hardware.vulkan.level-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml \
    frameworks/native/data/etc/android.hardware.vulkan.version-1_0_3.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.software.verified_boot.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.verified_boot.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.android.nfc_extras.xml \
    frameworks/native/data/etc/com.nxp.mifare.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.nxp.mifare.xml \
    frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml

# Power
PRODUCT_PACKAGES += \
    android.hardware.power@1.3-service.sony \
    librqbalance

# Public Libraries
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/public.libraries.txt:$(TARGET_COPY_OUT_VENDOR)/etc/public.libraries.txt

# QMI
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/dsi_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/data/dsi_config.xml \
    $(COMMON_PATH)/configs/netmgr_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/data/netmgr_config.xml \
    $(COMMON_PATH)/configs/qmi_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/data/qmi_config.xml

# RenderScript
PRODUCT_PACKAGES += \
    android.hardware.renderscript@1.0-impl \
    librsjni

# RIL
PRODUCT_PACKAGES += \
    android.hardware.radio@1.0 \
    android.hardware.radio@1.1 \
    android.hardware.radio.deprecated@1.0 \
    android.hardware.radio@1.0-service \
    libprotobuf-cpp-full \
    librmnetctl \
    Stk

# RQBalance-PowerHAL configuration
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/rqbalance_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/rqbalance_config.xml

# Rootdir
PRODUCT_PACKAGES += \
    fstab.qcom

PRODUCT_PACKAGES += \
    init.qcom.devstart.sh \
    init.qcom.ipastart.sh

PRODUCT_PACKAGES += \
    init.qcom.rc \
    init.qcom.power.rc \
    init.qcom.usb.rc \
    init.recovery.qcom.rc \
    ueventd.qcom.rc

PRODUCT_PACKAGES += \
    adsprpcd.rc \
    android.hardware.gnss@2.0-service-qti.rc \
    cdsprpcd.rc \
    irsc_util.rc \
    loc_launcher.rc \
    mlid.rc \
    mlog_qmi.rc \
    msm_irq.rc \
    netmgrd.rc \
    per-proxy.rc \
    per-service.rc \
    qmuxd.rc \
    qseecom.rc \
    rild2.rc \
    rmt_storage.rc \
    sct_service.rc \
    sensors.rc \
    tad.rc \
    ta_qmi.rc

# SAR
PRODUCT_PACKAGES += \
    TransPowerSensors

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/permissions/privapp-permissions-transmitpower.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/privapp-permissions-transmitpower.xml

# Sec Configuration
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Seccomp
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/seccomp/mediacodec.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy

# SEPolicy
include device/sony/sepolicy/sepolicy.mk

# Sensors
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/sensors_settings:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/sensors_settings

# Thermal HAL
PRODUCT_PACKAGES += \
    thermanager

# Touch HAL
PRODUCT_PACKAGES += \
    vendor.lineage.touch@1.0-service.loire

# Trust
PRODUCT_PACKAGES += \
    vendor.lineage.trust@1.0-service

# Usb HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0 \
    android.hardware.usb@1.0-service.basic

# Vibrator
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service

# VNDK
PRODUCT_PACKAGES += \
    vndk_package

PRODUCT_COPY_FILES += \
    prebuilts/vndk/v29/arm/arch-arm-armv7-a-neon/shared/vndk-core/libprotobuf-cpp-full.so:$(TARGET_COPY_OUT_VENDOR)/lib/libprotobuf-cpp-full-v29.so \
    prebuilts/vndk/v29/arm/arch-arm-armv7-a-neon/shared/vndk-core/libprotobuf-cpp-lite.so:$(TARGET_COPY_OUT_VENDOR)/lib/libprotobuf-cpp-lite-v29.so \
    prebuilts/vndk/v29/arm64/arch-arm64-armv8-a/shared/vndk-core/libprotobuf-cpp-full.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libprotobuf-cpp-full-v29.so \
    prebuilts/vndk/v29/arm64/arch-arm64-armv8-a/shared/vndk-core/libprotobuf-cpp-lite.so:$(TARGET_COPY_OUT_VENDOR)/lib64/libprotobuf-cpp-lite-v29.so

# WiFi
PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0 \
    android.hardware.wifi@1.1 \
    android.hardware.wifi@1.0-service.legacy \
    hostapd \
    libwpa_client \
    wpa_supplicant \
    wpa_supplicant.conf

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf \
    $(COMMON_PATH)/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf

# Vendor
$(call inherit-product-if-exists, vendor/sony/msm8956-common/msm8956-common-vendor.mk)
