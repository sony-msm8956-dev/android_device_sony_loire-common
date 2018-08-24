/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright (C) 2018 Shane Francis / Jens Andersen
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

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.1-service.loire"

#include "BiometricsFingerprint.h"

#include <byteswap.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <unistd.h>

#include "android-base/macros.h"

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_1 {
namespace implementation {

using RequestStatus = android::hardware::biometrics::fingerprint::V2_1::RequestStatus;

BiometricsFingerprint* BiometricsFingerprint::sInstance = nullptr;

BiometricsFingerprint::BiometricsFingerprint()
    : mWt(this), mClientCallback(nullptr), mDevice(nullptr) {
    fpc_imp_data_t* fpc_data = NULL;

    mDevice = (somc_fingerprint_device_t*)malloc(sizeof(somc_fingerprint_device_t));
    LOG_ALWAYS_FATAL_IF(!mDevice, "Failed to allocate somc_fingerprint_device_t");
    memset(mDevice, 0, sizeof(somc_fingerprint_device_t));

    if (fpc_init(&fpc_data, mWt.getEventFd()) < 0) {
        LOG_ALWAYS_FATAL("Could not init FPC device");
    }

    mDevice->fpc = fpc_data;

    mWt.Start();

    sInstance = this;  // keep track of the most recent instance
}

BiometricsFingerprint::~BiometricsFingerprint() {
    ALOGV("~BiometricsFingerprint()");
    if (mDevice == nullptr) {
        ALOGE("No valid device");
        return;
    }
    mDevice = nullptr;
}

Return<RequestStatus> BiometricsFingerprint::ErrorFilter(int32_t error) {
    switch (error) {
        case 0:
            return RequestStatus::SYS_OK;
        case -2:
            return RequestStatus::SYS_ENOENT;
        case -4:
            return RequestStatus::SYS_EINTR;
        case -5:
            return RequestStatus::SYS_EIO;
        case -11:
            return RequestStatus::SYS_EAGAIN;
        case -12:
            return RequestStatus::SYS_ENOMEM;
        case -13:
            return RequestStatus::SYS_EACCES;
        case -14:
            return RequestStatus::SYS_EFAULT;
        case -16:
            return RequestStatus::SYS_EBUSY;
        case -22:
            return RequestStatus::SYS_EINVAL;
        case -28:
            return RequestStatus::SYS_ENOSPC;
        case -110:
            return RequestStatus::SYS_ETIMEDOUT;
        default:
            ALOGE("An unknown error returned from fingerprint vendor library: %d", error);
            return RequestStatus::SYS_UNKNOWN;
    }
}

Return<uint64_t> BiometricsFingerprint::setNotify(
        const sp<IBiometricsFingerprintClientCallback>& clientCallback) {
    std::lock_guard<std::mutex> lock(mClientCallbackMutex);
    mClientCallback = clientCallback;
    // This is here because HAL 2.1 doesn't have a way to propagate a
    // unique token for its driver. Subsequent versions should send a unique
    // token for each call to setNotify(). This is fine as long as there's only
    // one fingerprint device on the platform.
    return reinterpret_cast<uint64_t>(mDevice);
}

Return<uint64_t> BiometricsFingerprint::preEnroll() {
    mDevice->challenge = fpc_load_auth_challenge(mDevice->fpc);
    ALOGI("%s: Challenge is: %ju", __func__, mDevice->challenge);
    return mDevice->challenge;
}

Return<RequestStatus> BiometricsFingerprint::enroll(const hidl_array<uint8_t, 69>& hat,
                                                    uint32_t gid ATTRIBUTE_UNUSED,
                                                    uint32_t timeoutSec ATTRIBUTE_UNUSED) {
    const hw_auth_token_t* authToken = reinterpret_cast<const hw_auth_token_t*>(hat.data());

    if (!mWt.Pause()) {
        return RequestStatus::SYS_EBUSY;
    }

    ALOGI("%s: hat->challenge %lu", __func__, (unsigned long)authToken->challenge);
    ALOGI("%s: hat->user_id %lu", __func__, (unsigned long)authToken->user_id);
    ALOGI("%s: hat->authenticator_id %lu", __func__, (unsigned long)authToken->authenticator_id);
    ALOGI("%s: hat->authenticator_type %d", __func__, authToken->authenticator_type);
    ALOGI("%s: hat->timestamp %lu", __func__, (unsigned long)authToken->timestamp);
    ALOGI("%s: hat size %lu", __func__, (unsigned long)sizeof(hw_auth_token_t));

    int rc = fpc_verify_auth_challenge(mDevice->fpc, (void*)authToken, sizeof(hw_auth_token_t));
    if (rc) {
        return ErrorFilter(rc);
    }

    bool success = mWt.waitForState(AsyncState::Enroll);
    return success ? RequestStatus::SYS_OK : RequestStatus::SYS_EAGAIN;
}

Return<RequestStatus> BiometricsFingerprint::postEnroll() {
    ALOGI("%s: Resetting challenge", __func__);
    mDevice->challenge = 0;
    return RequestStatus::SYS_OK;
}

Return<uint64_t> BiometricsFingerprint::getAuthenticatorId() {
    uint64_t id = fpc_load_db_id(mDevice->fpc);
    ALOGI("%s: ID: %ju", __func__, id);
    return id;
}

Return<RequestStatus> BiometricsFingerprint::cancel() {
    ALOGI("%s", __func__);

    if (mWt.Resume()) {
        ALOGI("%s: Successfully moved to pause state", __func__);
        return RequestStatus::SYS_OK;
    }

    ALOGE("%s: Failed to move to pause state", __func__);
    return RequestStatus::SYS_UNKNOWN;
}

Return<RequestStatus> BiometricsFingerprint::enumerate() {
    const uint64_t devId = reinterpret_cast<uint64_t>(mDevice);

    if (mClientCallback == nullptr) {
        ALOGE("Client callback not set");
        return RequestStatus::SYS_EFAULT;
    }

    ALOGV(__func__);

    if (!mWt.Pause()) {
        return RequestStatus::SYS_EBUSY;
    }

    fpc_fingerprint_index_t print_indices;
    int rc = fpc_get_print_index(mDevice->fpc, &print_indices);

    if (!rc) {
        if (!print_indices.print_count) {
            // When there are no fingers, the service still needs to know that (potentially async)
            // enumeration has finished. By convention, send fid=0 and remaining=0 to signal this:
            mClientCallback->onEnumerate(devId, 0, mDevice->gid, 0);
        } else {
            for (size_t i = 0; i < print_indices.print_count; i++) {
                ALOGD("%s: found print: %lu at index %zu", __func__,
                      (unsigned long)print_indices.prints[i], i);

                uint32_t remaining_templates = (uint32_t)(print_indices.print_count - i - 1);

                mClientCallback->onEnumerate(devId, print_indices.prints[i], mDevice->gid,
                                             remaining_templates);
            }
        }
    }

    mWt.Resume();

    return ErrorFilter(rc);
}

Return<RequestStatus> BiometricsFingerprint::remove(uint32_t gid, uint32_t fid) {
    const uint64_t devId = reinterpret_cast<uint64_t>(mDevice);
    int rc = 0;

    if (mClientCallback == nullptr) {
        ALOGE("Client callback not set");
        return RequestStatus::SYS_EINVAL;
    }

    if (!mWt.Pause()) {
        return RequestStatus::SYS_EBUSY;
    }

    if (fid == 0) {
        // Delete all fingerprints when fid is zero
        ALOGD("Deleting all fingerprints for gid %d", gid);

        fpc_fingerprint_index_t print_indices;
        rc = fpc_get_print_index(mDevice->fpc, &print_indices);
        if (!rc) {
            for (auto remaining = print_indices.print_count; remaining--;) {
                auto fid = print_indices.prints[remaining];
                ALOGD("Deleting print %d, %d remaining", fid, remaining);
                rc = fpc_del_print_id(mDevice->fpc, fid);
                if (rc) {
                    break;
                }
                mClientCallback->onRemoved(devId, fid, gid, remaining);
            }
        }
    } else {
        ALOGD("Removing finger %u for gid %u", fid, gid);
        rc = fpc_del_print_id(mDevice->fpc, fid);
        if (!rc) {
            mClientCallback->onRemoved(devId, fid, gid, 0);
        }
    }

    if (rc) {
        mClientCallback->onError(devId, FingerprintError::ERROR_UNABLE_TO_REMOVE, 0);
    } else {
        uint32_t db_length = fpc_get_user_db_length(mDevice->fpc);
        ALOGD("%s: User database length is: %u", __func__, db_length);
        rc = fpc_store_user_db(mDevice->fpc, db_length, mDevice->db_path);
    }

    mWt.Resume();

    return ErrorFilter(rc);
}

int BiometricsFingerprint::__setActiveGroup(uint32_t gid) {
    int result;
    bool created_empty_db = false;
    struct stat sb;

    if (stat(mDevice->db_path, &sb) == -1) {
        // No existing database, load an empty one
        if ((result = fpc_load_empty_db(mDevice->fpc)) != 0) {
            ALOGE("Error creating empty user database: %d\n", result);
            return result;
        }
        created_empty_db = true;
    } else {
        if ((result = fpc_load_user_db(mDevice->fpc, mDevice->db_path)) != 0) {
            ALOGE("Error loading existing user database: %d\n", result);
            return result;
        }
    }

    if ((result = fpc_set_gid(mDevice->fpc, gid)) != 0) {
        ALOGE("Error setting current gid: %d\n", result);
    }

    // if user database was created in this instance, store it directly
    if (created_empty_db) {
        int length = fpc_get_user_db_length(mDevice->fpc);
        fpc_store_user_db(mDevice->fpc, length, mDevice->db_path);
        if ((result = fpc_load_user_db(mDevice->fpc, mDevice->db_path)) != 0) {
            ALOGE("Error loading empty user database: %d\n", result);
            return result;
        }
    }
    return result;
}

Return<RequestStatus> BiometricsFingerprint::setActiveGroup(uint32_t gid,
                                                            const hidl_string& storePath) {
    int result;

    if (storePath.size() >= PATH_MAX || storePath.size() <= 0) {
        ALOGE("Bad path length: %zd", storePath.size());
        return RequestStatus::SYS_EINVAL;
    }
    if (access(storePath.c_str(), W_OK)) {
        return RequestStatus::SYS_EINVAL;
    }

    sprintf(mDevice->db_path, "%s/user.db", storePath.c_str());
    mDevice->gid = gid;

    ALOGI("%s: storage path set to: %s", __func__, mDevice->db_path);

    if (!mWt.Pause()) {
        return RequestStatus::SYS_EBUSY;
    }

    result = __setActiveGroup(gid);

    mWt.Resume();

    return ErrorFilter(result);
}

Return<RequestStatus> BiometricsFingerprint::authenticate(uint64_t operation_id,
                                                          uint32_t gid ATTRIBUTE_UNUSED) {
    err_t r;

    ALOGI("%s: operation_id=%ju", __func__, operation_id);

    if (!mWt.Pause()) {
        return RequestStatus::SYS_EBUSY;
    }

    r = fpc_set_auth_challenge(mDevice->fpc, operation_id);
    auth_challenge = operation_id;
    if (r < 0) {
        ALOGE("%s: Error setting auth challenge to %ju. r=0x%08X", __func__, operation_id, r);
        return RequestStatus::SYS_EAGAIN;
    }

    bool success = mWt.waitForState(AsyncState::Authenticate);
    return success ? RequestStatus::SYS_OK : RequestStatus::SYS_EAGAIN;
}

IBiometricsFingerprint* BiometricsFingerprint::getInstance() {
    if (!sInstance) {
        new BiometricsFingerprint();
    }
    return sInstance;
}

void BiometricsFingerprint::IdleAsync() {
    ALOGD(__func__);

    WorkHandler::IdleAsync();

    return;
}

void BiometricsFingerprint::EnrollAsync() {
    int32_t print_count = 0;
    const uint64_t devId = reinterpret_cast<uint64_t>(mDevice);

    std::lock_guard<std::mutex> lock(mClientCallbackMutex);
    if (mClientCallback == nullptr) {
        ALOGE("Receiving callbacks before the client callback is registered.");
        return;
    }

    if (fpc_set_power(&mDevice->fpc->event, FPC_PWRON) < 0) {
        ALOGE("Error starting device");
        mClientCallback->onError(devId, FingerprintError::ERROR_UNABLE_TO_PROCESS, 0);
        return;
    }

    int ret = fpc_enroll_start(mDevice->fpc, print_count);
    if (ret < 0) {
        ALOGE("Starting enroll failed: %d\n", ret);
    }

    int status = 1;
    while ((status = fpc_capture_image(mDevice->fpc)) >= 0) {
        ALOGD("%s: Got input status=%d", __func__, status);

        if (mWt.isEventAvailable()) {
            mClientCallback->onError(devId, FingerprintError::ERROR_CANCELED, 0);
            break;
        }

        FingerprintAcquiredInfo hidlStatus = (FingerprintAcquiredInfo)status;

        if (hidlStatus <= FingerprintAcquiredInfo::ACQUIRED_TOO_FAST)
            mClientCallback->onAcquired(devId, hidlStatus, 0);

        // image captured
        if (status == FINGERPRINT_ACQUIRED_GOOD) {
            ALOGI("%s: Enroll Step", __func__);
            uint32_t remaining_touches = 0;
            int ret = fpc_enroll_step(mDevice->fpc, &remaining_touches);
            ALOGI("%s: step: %d, touches=%d\n", __func__, ret, remaining_touches);
            if (ret > 0) {
                ALOGI("%s: Touches Remaining: %d", __func__, remaining_touches);
                if (remaining_touches > 0) {
                    mClientCallback->onEnrollResult(devId, 0, 0, remaining_touches);
                }
            } else if (ret == 0) {
                uint32_t print_id = 0;
                int print_index = fpc_enroll_end(mDevice->fpc, &print_id);

                if (print_index < 0) {
                    ALOGE("%s: Error getting new print index: %d", __func__, print_index);
                    mClientCallback->onError(devId, FingerprintError::ERROR_UNABLE_TO_PROCESS, 0);
                    break;
                }

                uint32_t db_length = fpc_get_user_db_length(mDevice->fpc);
                ALOGI("%s: User database length is: %lu", __func__, (unsigned long)db_length);
                fpc_store_user_db(mDevice->fpc, db_length, mDevice->db_path);
                ALOGI("%s: Got print id: %lu", __func__, (unsigned long)print_id);
                mClientCallback->onEnrollResult(devId, print_id, mDevice->gid, 0);
                break;
            } else {
                ALOGE("Error in enroll step, aborting enroll: %d\n", ret);
                mClientCallback->onError(devId, FingerprintError::ERROR_UNABLE_TO_PROCESS, 0);
                break;
            }
        }
    }
    if (fpc_set_power(&mDevice->fpc->event, FPC_PWROFF) < 0) {
        ALOGE("Error stopping device");
    }

    if (status < 0) {
        mClientCallback->onError(devId, FingerprintError::ERROR_HW_UNAVAILABLE, 0);
    }
}

void BiometricsFingerprint::AuthenticateAsync() {
    int result;
    int status = 1;
    const uint64_t devId = reinterpret_cast<uint64_t>(mDevice);

    std::lock_guard<std::mutex> lock(mClientCallbackMutex);
    if (mClientCallback == nullptr) {
        ALOGE("Receiving callbacks before the client callback is registered.");
        return;
    }

    if (fpc_set_power(&mDevice->fpc->event, FPC_PWRON) < 0) {
        ALOGE("Error starting device");
        mClientCallback->onError(devId, FingerprintError::ERROR_UNABLE_TO_PROCESS, 0);
        return;
    }

    fpc_auth_start(mDevice->fpc);

    while ((status = fpc_capture_image(mDevice->fpc)) >= 0) {
        ALOGV("%s: Got input with status %d", __func__, status);

        if (mWt.isEventAvailable()) {
            mClientCallback->onError(devId, FingerprintError::ERROR_CANCELED, 0);
            break;
        }

        FingerprintAcquiredInfo hidlStatus = (FingerprintAcquiredInfo)status;

        if (hidlStatus <= FingerprintAcquiredInfo::ACQUIRED_TOO_FAST) {
            mClientCallback->onAcquired(devId, hidlStatus, 0);
        }

        if (status == FINGERPRINT_ACQUIRED_GOOD) {
            uint32_t print_id = 0;
            int verify_state = fpc_auth_step(mDevice->fpc, &print_id);
            ALOGI("%s: Auth step = %d", __func__, verify_state);

            /* After getting something that ought to have been
             * recognizable: Either send proper notification, or
             * dummy one where fid=zero stands for unrecognized.
             */
            uint32_t gid = mDevice->gid;
            uint32_t fid = 0;

            if (verify_state >= 0) {
                result = fpc_update_template(mDevice->fpc);
                if (result < 0) {
                    ALOGE("Error updating template: %d", result);
                } else if (result) {
                    ALOGI("Storing db");
                    result = fpc_store_user_db(mDevice->fpc, 0, mDevice->db_path);
                    if (result) {
                        ALOGE("Error storing database: %d", result);
                    }
                }

                if (print_id > 0) {
                    hw_auth_token_t hat;
                    ALOGI("%s: Got print id: %u", __func__, print_id);

                    if (auth_challenge) {
                        fpc_get_hw_auth_obj(mDevice->fpc, &hat, sizeof(hw_auth_token_t));

                        ALOGI("%s: hat->challenge %ju", __func__, hat.challenge);
                        ALOGI("%s: hat->user_id %ju", __func__, hat.user_id);
                        ALOGI("%s: hat->authenticator_id %ju", __func__, hat.authenticator_id);
                        ALOGI("%s: hat->authenticator_type %u", __func__,
                              ntohl(hat.authenticator_type));
                        ALOGI("%s: hat->timestamp %lu", __func__, bswap_64(hat.timestamp));
                        ALOGI("%s: hat size %zu", __func__, sizeof(hw_auth_token_t));
                    } else {
                        // Without challenge, there's no reason to bother the TZ to
                        // provide an "invalid" response token.
                        ALOGD("No authentication challenge set. Reporting empty HAT");
                        memset(&hat, 0, sizeof(hat));
                    }

                    fid = print_id;

                    const uint8_t* hat2 = reinterpret_cast<const uint8_t*>(&hat);
                    const hidl_vec<uint8_t> token(std::vector<uint8_t>(hat2, hat2 + sizeof(hat)));

                    mClientCallback->onAuthenticated(devId, fid, gid, token);
                    break;
                } else {
                    ALOGI("%s: Got print id: %u", __func__, print_id);
                    mClientCallback->onAuthenticated(devId, fid, gid, hidl_vec<uint8_t>());
                }
            } else if (verify_state == -EAGAIN) {
                ALOGI("%s: retrying due to receiving -EAGAIN", __func__);
                mClientCallback->onAuthenticated(devId, fid, gid, hidl_vec<uint8_t>());
            } else {
                /*
                 * Reinitialize the TZ app and parameters
                 * to clear the TZ error generated by flooding it
                 */
                result = fpc_close(&mDevice->fpc);
                LOG_ALWAYS_FATAL_IF(result < 0, "Reinitialize: Failed to close fpc: %d", result);
                result = fpc_init(&mDevice->fpc, mWt.getEventFd());
                LOG_ALWAYS_FATAL_IF(result < 0, "Reinitialize: Failed to init fpc: %d", result);
                // Break out of the loop, and make sure ERROR_HW_UNAVAILABLE
                // is raised afterwards, similar to the stock hal:
                status = -1;
                break;
            }
        }
    }
    if (fpc_set_power(&mDevice->fpc->event, FPC_PWROFF) < 0) {
        ALOGE("Error stopping device");
    }

    if (status < 0) {
        mClientCallback->onError(devId, FingerprintError::ERROR_HW_UNAVAILABLE, 0);
    }
}

}  // namespace implementation
}  // namespace V2_1
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
