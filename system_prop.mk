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

# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicecomm=true \
    persist.audio.fluence.voicerec=false \
    persist.audio.fluence.speaker=true \
    media.aac_51_output_enabled=true \
    audio.deep_buffer.media=1 \
    fmas.hdph_sgain=0 \
    ro.config.vc_call_vol_steps=7 \
    ro.config.media_vol_steps=25 \
    ro.qc.sdk.audio.fluencetype=fluence

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bt.bdaddr_path=/data/vendor/bluetooth/bluetooth_bdaddr

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    persist.camera.gyro.disable=1 \
    persist.camera.feature.cac=0 \
    persist.camera.ois.disable=0 \
    persist.camera.eis.enable=0 \
    persist.camera.zsl.mode=1 \
    persist.camera.exif.rotation=off \
    persist.camera.lib2d.rotation=on \
    persist.camera.dual.camera.mpo=0 \
    persist.camera.dc.frame.sync=0 \
    persist.camera.dcrf.enable=0 \
    persist.camera.hal.debug=0 \
    persist.camera.kpi.debug=0 \
    persist.camera.global.debug=0

# Dalvik
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=16m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=4m \
    dalvik.vm.heapmaxfree=8m

# DRM service
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

# Dex2Oat
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dex2oat-threads=2 \
    dalvik.vm.image-dex2oat-threads=4

# Graphics
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sf.latch_unsignaled=1 \
    ro.opengles.version=196610 \
    ro.qcom.adreno.qgl.ShaderStorageImageExtendedFormats=0 \
    persist.hwc2.skip_client_color_transform=false

# IRQBalance
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=/vendor/lib/librqbalance.so

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    debug.gralloc.gfx_ubwc_disable=1 \
    debug.gralloc.gfx_ubwc_disable_=1 \
    debug.gralloc.enable_fb_ubwc=0 \
    media.msm8956hw=1 \
    vidc.debug.level=1 \
    vidc.dec.enable.downscalar=1 \
    vidc.dec.downscalar_width=1920 \
    vidc.dec.downscalar_height=1088 \
    vidc.enc.disable_bframes=1 \
    vidc.enc.disable_pframes=1 \
    vidc.disable.split.mode=1 \
    video.disable.ubwc=1

# NFC
PRODUCT_PROPERTY_OVERRIDES += \
    ro.nfc.port=I2C

# Priv-app permisisons
PRODUCT_PROPERTY_OVERRIDES += \
    ro.control_privapp_permissions=enforce

# QMI
PRODUCT_PROPERTY_OVERRIDES += \
    persist.data.qmi.adb_logmask=0

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.default_network=9 \
    rild.libpath=/vendor/lib64/libril-qc-qmi-1.so \
    ril.subscription.types=NV,RUIM \
    ro.use_data_netmgrd=true \
    persist.data.netmgrd.qos.enable=true \
    ro.data.large_tcp_window_size=true \
    telephony.lteOnCdmaDevice=0 \
    ro.telephony.call_ring.multiple=false \
    persist.radio.add_power_save=1 \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.oem_socket=false

# Security patch level
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.build.security_patch=2019-03-01

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    ro.qti.sensors.max_accel_rate=false \
    ro.qti.sensors.max_gyro_rate=false \
    ro.qti.sensors.max_mag_rate=false \
    ro.qti.sensors.max_geomag_rotv=50 \
    ro.qti.sdk.sensors.gestures=false \
    ro.qti.sensors.pedometer=false \
    ro.qti.sensors.step_detector=true \
    ro.qti.sensors.step_counter=true \
    ro.qti.sensors.pam=false \
    ro.qti.sensors.scrn_ortn=false \
    ro.qti.sensors.smd=true \
    ro.qti.sensors.game_rv=true \
    ro.qti.sensors.georv=true \
    ro.qti.sensors.cmc=false \
    ro.qti.sensors.bte=false \
    ro.qti.sensors.fns=false \
    ro.qti.sensors.qmd=false \
    ro.qti.sensors.amd=false \
    ro.qti.sensors.rmd=false \
    ro.qti.sensors.vmd=false \
    ro.qti.sensors.gtap=false \
    ro.qti.sensors.tap=false \
    ro.qti.sensors.facing=false \
    ro.qti.sensors.tilt=false \
    ro.qti.sensors.tilt_detector=true \
    ro.qti.sensors.dpc=false \
    ro.qti.sensors.als_scale=1 \
    ro.qti.sensors.wu=true \
    persist.debug.sensors.hal=0 \
    debug.qualcomm.sns.daemon=0 \
    debug.qualcomm.sns.hal=0 \
    debug.qualcomm.sns.libsensor1=0

# USB
PRODUCT_PROPERTY_OVERRIDES += \
    sys.usb.controller=msm_hsusb \
    sys.usb.rndis.func.name=rndis_bam

# WFD
PRODUCT_PROPERTY_OVERRIDES += \
    persist.debug.wfd.enable=1 \
    persist.sys.wfd.virtual=0

# WiFi
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0 \
    ro.vendor.wifi.addr_path=/sys/devices/soc/soc:bcmdhd_wlan/macaddr
