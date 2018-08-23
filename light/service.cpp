/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2018-2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.light@2.0-service.loire"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>

#include "Light.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::light::V2_0::ILight;
using android::hardware::light::V2_0::implementation::Light;
using namespace android;

const static std::string kLcdBacklightPath = "/sys/class/leds/lcd-backlight/brightness";
const static std::string kLcdMaxBacklightPath = "/sys/class/leds/lcd-backlight/max_brightness";
const static std::string kRedLedPath = "/sys/class/leds/as3668:red/brightness";
const static std::string kRedPatternPath = "/sys/class/leds/as3668:red/pattern_brightness";
const static std::string kRedPatternFrameMaskPath = "/sys/class/leds/as3668:red/pattern_frame_mask";
const static std::string kGreenLedPath = "/sys/class/leds/as3668:green/brightness";
const static std::string kGreenPatternPath = "/sys/class/leds/as3668:green/pattern_brightness";
const static std::string kGreenPatternFrameMaskPath = "/sys/class/leds/as3668:green/pattern_frame_mask";
const static std::string kBlueLedPath = "/sys/class/leds/as3668:blue/brightness";
const static std::string kBluePatternPath = "/sys/class/leds/as3668:blue/pattern_brightness";
const static std::string kBluePatternFrameMaskPath = "/sys/class/leds/as3668:blue/pattern_frame_mask";
const static std::string kPatternPulseNumberPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_multiple_pulse_number";
const static std::string kPatternDimSpeedDownPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_pwm_dim_speed_down_ms";
const static std::string kPatternDimSpeedUpPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_pwm_dim_speed_up_ms";
const static std::string kPatternTimeOffPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_time_off_ms";
const static std::string kPatternTimeOnPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_time_on_ms";
const static std::string kPatternRunPath = "/sys/class/i2c-dev/i2c-6/device/6-0042/pattern_run";

status_t registerLightService() {
    uint32_t lcdMaxBrightness = 255;

    std::ofstream lcdBacklight(kLcdBacklightPath);
    if (!lcdBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ifstream lcdMaxBacklight(kLcdMaxBacklightPath);
    if (!lcdMaxBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdMaxBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    } else {
        lcdMaxBacklight >> lcdMaxBrightness;
    }

    std::ofstream redLed(kRedLedPath);
    if (!redLed) {
        LOG(ERROR) << "Failed to open " << kRedLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenLed(kGreenLedPath);
    if (!greenLed) {
        LOG(ERROR) << "Failed to open " << kGreenLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueLed(kBlueLedPath);
    if (!blueLed) {
        LOG(ERROR) << "Failed to open " << kBlueLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redPattern(kRedPatternPath);
    if (!redPattern) {
        LOG(ERROR) << "Failed to open " << kRedPatternPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenPattern(kGreenPatternPath);
    if (!greenPattern) {
        LOG(ERROR) << "Failed to open " << kGreenPatternPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePattern(kBluePatternPath);
    if (!bluePattern) {
        LOG(ERROR) << "Failed to open " << kBluePatternPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redPatternFrameMask(kRedPatternFrameMaskPath);
    if (!redPatternFrameMask) {
        LOG(ERROR) << "Failed to open " << kRedPatternFrameMaskPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenPatternFrameMask(kGreenPatternFrameMaskPath);
    if (!greenPatternFrameMask) {
        LOG(ERROR) << "Failed to open " << kGreenPatternFrameMaskPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePatternFrameMask(kBluePatternFrameMaskPath);
    if (!bluePatternFrameMask) {
        LOG(ERROR) << "Failed to open " << kBluePatternFrameMaskPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternPulseNumber(kPatternPulseNumberPath);
    if (!patternPulseNumber) {
        LOG(ERROR) << "Failed to open " << kPatternPulseNumberPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternDimSpeedDown(kPatternDimSpeedDownPath);
    if (!patternDimSpeedDown) {
        LOG(ERROR) << "Failed to open " << kPatternDimSpeedDownPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternDimSpeedUp(kPatternDimSpeedUpPath);
    if (!patternDimSpeedUp) {
        LOG(ERROR) << "Failed to open " << kPatternDimSpeedUpPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternTimeOff(kPatternTimeOffPath);
    if (!patternTimeOff) {
        LOG(ERROR) << "Failed to open " << kPatternTimeOffPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternTimeOn(kPatternTimeOnPath);
    if (!patternTimeOn) {
        LOG(ERROR) << "Failed to open " << kPatternTimeOnPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream patternRun(kPatternRunPath);
    if (!patternRun) {
        LOG(ERROR) << "Failed to open " << kPatternRunPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    sp<ILight> light = new Light({std::move(lcdBacklight), lcdMaxBrightness},
            std::move(redLed), std::move(greenLed), std::move(blueLed),
            std::move(redPattern), std::move(greenPattern), std::move(bluePattern),
            std::move(redPatternFrameMask), std::move(greenPatternFrameMask),
            std::move(bluePatternFrameMask),std::move(patternPulseNumber),
            std::move(patternDimSpeedDown), std::move(patternDimSpeedUp),
            std::move(patternTimeOff), std::move(patternTimeOn), std::move(patternRun));

    return light->registerAsService();
}

int main() {
    status_t status;

    configureRpcThreadpool(1, true /*willJoinThreadpool*/);

    status = registerLightService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for Light HAL (" << status << ")";
        goto shutdown;
    }

    LOG(INFO) << "Light HAL service is ready.";
    joinRpcThreadpool();

shutdown:
    // In normal operation, we don't expect the thread pool to exit
    LOG(ERROR) << "Light HAL service is shutting down.";
    return 1;
}
