/*
 * Copyright (C) 2021 The LineageOS Project
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

#define LOG_TAG "QTI PowerHAL"
#include <log/log.h>

#include <aidl/android/hardware/power/BnPower.h>

extern "C" {
#include "hint-data.h"
#include "metadata-defs.h"
#include "performance.h"
#include "power-common.h"
#include "utils.h"

const int kMinInteractiveDuration = 500;  /* ms */
const int kMaxInteractiveDuration = 5000; /* ms */

static void process_interaction_hint(void *data) {
  static struct timespec s_previous_boost_timespec;
  static int s_previous_duration = 0;
  static int interaction_handle = -1;

  struct timespec cur_boost_timespec;
  long long elapsed_time;
  int duration = kMinInteractiveDuration;

  if (data) {
    int input_duration = *((int *)data);
    if (input_duration > duration) {
      duration = (input_duration > kMaxInteractiveDuration)
                     ? kMaxInteractiveDuration
                     : input_duration;
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &cur_boost_timespec);

  elapsed_time =
      calc_timespan_us(s_previous_boost_timespec, cur_boost_timespec);
  // don't hint if it's been less than 250ms since last boost
  // also detect if we're doing anything resembling a fling
  // support additional boosting in case of flings
  if (elapsed_time < 250000 && duration <= 750) {
    return;
  }
  s_previous_boost_timespec = cur_boost_timespec;
  s_previous_duration = duration;

  if (CHECK_HANDLE(interaction_handle)) {
    release_request(interaction_handle);
    interaction_handle = -1;
  }

  interaction_handle = perf_hint_enable_with_type(VENDOR_HINT_SCROLL_BOOST,
                                                  duration, SCROLL_VERTICAL);
}
}

using ::aidl::android::hardware::power::Boost;

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {

bool isDeviceSpecificBoostSupported(Boost type, bool *_aidl_return) {
  switch (type) {
  case Boost::INTERACTION:
    *_aidl_return = true;
    return true;
  default:
    return false;
  }
}

bool setDeviceSpecificBoost(Boost type, int32_t durationMs) {
  switch (type) {
  case Boost::INTERACTION:
    process_interaction_hint(&durationMs);
    return true;
  default:
    return false;
  }
}

} // namespace impl
} // namespace power
} // namespace hardware
} // namespace android
} // namespace aidl
