/*
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

#define LOG_TAG "LightService"

#include "Light.h"

#include <android-base/logging.h>

namespace {
using android::hardware::light::V2_0::LightState;

static constexpr int DEFAULT_MAX_BRIGHTNESS = 255;

static uint32_t rgbToBrightness(const LightState& state) {
    uint32_t color = state.color & 0x00ffffff;
    return ((77 * ((color >> 16) & 0xff)) + (150 * ((color >> 8) & 0xff)) +
            (29 * (color & 0xff))) >> 8;
}

static bool isLit(const LightState& state) {
    return (state.color & 0x00ffffff);
}
}  // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Light::Light(std::pair<std::ofstream, uint32_t>&& lcd_backlight,
         std::ofstream&& red_led, std::ofstream&& green_led, std::ofstream&& blue_led,
         std::ofstream&& red_pattern, std::ofstream&& green_pattern, std::ofstream&& blue_pattern,
         std::ofstream&& red_pattern_frame_mask, std::ofstream&& green_pattern_frame_mask,
         std::ofstream&& blue_pattern_frame_mask, std::ofstream&& pattern_pulse_number,
         std::ofstream&& pattern_dim_speed_down, std::ofstream&& pattern_dim_speed_up,
         std::ofstream&& pattern_time_off, std::ofstream&& pattern_time_on,
         std::ofstream&& pattern_run)
    : mLcdBacklight(std::move(lcd_backlight)),
      mRedLed(std::move(red_led)),
      mGreenLed(std::move(green_led)),
      mBlueLed(std::move(blue_led)),
      mRedPattern(std::move(red_pattern)),
      mGreenPattern(std::move(green_pattern)),
      mBluePattern(std::move(blue_pattern)),
      mRedPatternFrameMask(std::move(red_pattern_frame_mask)),
      mGreenPatternFrameMask(std::move(green_pattern_frame_mask)),
      mBluePatternFrameMask(std::move(blue_pattern_frame_mask)),
      mPatternPulseNumber(std::move(pattern_pulse_number)),
      mPatternDimSpeedDown(std::move(pattern_dim_speed_down)),
      mPatternDimSpeedUp(std::move(pattern_dim_speed_up)),
      mPatternTimeOff(std::move(pattern_time_off)),
      mPatternTimeOn(std::move(pattern_time_on)),
      mPatternRun(std::move(pattern_run)) {
    auto attnFn(std::bind(&Light::setAttentionLight, this, std::placeholders::_1));
    auto backlightFn(std::bind(&Light::setLcdBacklight, this, std::placeholders::_1));
    auto batteryFn(std::bind(&Light::setBatteryLight, this, std::placeholders::_1));
    auto notifFn(std::bind(&Light::setNotificationLight, this, std::placeholders::_1));
    mLights.emplace(std::make_pair(Type::ATTENTION, attnFn));
    mLights.emplace(std::make_pair(Type::BACKLIGHT, backlightFn));
    mLights.emplace(std::make_pair(Type::BATTERY, batteryFn));
    mLights.emplace(std::make_pair(Type::NOTIFICATIONS, notifFn));
}

// Methods from ::android::hardware::light::V2_0::ILight follow.
Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = mLights.find(type);

    if (it == mLights.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    it->second(state);

    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (auto const& light : mLights) {
        types.push_back(light.first);
    }

    _hidl_cb(types);

    return Void();
}

void Light::setAttentionLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mAttentionState = state;
    setSpeakerBatteryLightLocked();
}

void Light::setLcdBacklight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);

    uint32_t brightness = rgbToBrightness(state);

    // If max panel brightness is not the default (255),
    // apply linear scaling across the accepted range.
    if (mLcdBacklight.second != DEFAULT_MAX_BRIGHTNESS) {
        int old_brightness = brightness;
        brightness = brightness * mLcdBacklight.second / DEFAULT_MAX_BRIGHTNESS;
        LOG(VERBOSE) << "scaling brightness " << old_brightness << " => " << brightness;
    }

    mLcdBacklight.first << brightness << std::endl;
}

void Light::setBatteryLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mBatteryState = state;
    setSpeakerBatteryLightLocked();
}

void Light::setNotificationLight(const LightState& state) {
    std::lock_guard<std::mutex> lock(mLock);
    mNotificationState = state;
    setSpeakerBatteryLightLocked();
}

void Light::setSpeakerBatteryLightLocked() {
    if (isLit(mNotificationState)) {
        setSpeakerLightLocked(mNotificationState);
    } else if (isLit(mAttentionState)) {
        setSpeakerLightLocked(mAttentionState);
    } else if (isLit(mBatteryState)) {
        setSpeakerLightLocked(mBatteryState);
    } else {
        // Lights off
        mRedLed << 0 << std::endl;
        mGreenLed << 0 << std::endl;
        mBlueLed << 0 << std::endl;
        mRedPattern << 0 << std::endl;
        mGreenPattern << 0 << std::endl;
        mBluePattern << 0 << std::endl;
    }
}

void Light::setSpeakerLightLocked(const LightState& state) {
    int red, green, blue, blink;
    int onMs, offMs;
    uint32_t alpha;

    // Extract brightness from AARRGGBB
    alpha = (state.color >> 24) & 0xff;

    // Retrieve each of the RGB colors
    red = (state.color >> 16) & 0xff;
    green = (state.color >> 8) & 0xff;
    blue = state.color & 0xff;

    // Scale RGB colors if a brightness has been applied by the user
    if (alpha != 0xff) {
        red = (red * alpha) / 0xff;
        green = (green * alpha) / 0xff;
        blue = (blue * alpha) / 0xff;
    }

    switch (state.flashMode) {
        case Flash::TIMED:
            onMs = state.flashOnMs;
            offMs = state.flashOffMs;
            break;
        case Flash::NONE:
        default:
            onMs = 0;
            offMs = 0;
            break;
    }
    blink = onMs > 0 && offMs > 0;

    // Disable pattern before starting
    mPatternRun << 0 << std::endl;

    if (blink) {
        mRedPattern << red << std::endl;
        mRedPatternFrameMask << 1 << std::endl;
        mGreenPattern << green << std::endl;
        mGreenPatternFrameMask << 1 << std::endl;
        mBluePattern << blue << std::endl;
        mBluePatternFrameMask << 1 << std::endl;
        mPatternPulseNumber << 1 << std::endl;
        mPatternDimSpeedDown << 1200 << std::endl;
        mPatternDimSpeedUp << 1200 << std::endl;
        mPatternTimeOff << offMs << std::endl;
        mPatternTimeOn << onMs << std::endl;

        // Enable pattern for blinking
        mPatternRun << 1 << std::endl;
    } else {
        if (red == 0 && green == 0 && blue == 0) {
            mRedPattern << 0 << std::endl;
            mGreenPattern << 0 << std::endl;
            mBluePattern << 0 << std::endl;
        }
        mRedLed << red << std::endl;
        mGreenLed << green << std::endl;
        mBlueLed << blue << std::endl;
    }
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
