/*
 * Copyright (C) 2016 Shane Francis / Jens Andersen
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

#ifndef __FINGERPRINTSCANNER_H_
#define __FINGERPRINTSCANNER_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FPC_IOC_MAGIC 0x1145
#define FPC_IOCWPREPARE _IOW(FPC_IOC_MAGIC, 0x01, int)
#define FPC_IOCWDEVWAKE _IOW(FPC_IOC_MAGIC, 0x02, int)
#define FPC_IOCWRESET _IOW(FPC_IOC_MAGIC, 0x03, int)
#define FPC_IOCRPREPARE _IOR(FPC_IOC_MAGIC, 0x81, int)
#define FPC_IOCRDEVWAKE _IOR(FPC_IOC_MAGIC, 0x82, int)
#define FPC_IOCRIRQ _IOR(FPC_IOC_MAGIC, 0x83, int)
#define FPC_IOCRIRQPOLL _IOR(FPC_IOC_MAGIC, 0x84, int)

#define FP_TZAPP_PATH "/system/etc/firmware/"
#define FP_TZAPP_NAME "tzfingerprint"

#define KM_TZAPP_PATH "/firmware/image/"
#define KM_TZAPP_NAME "keymaste"
#define KM_TZAPP_ALT_NAME "keymaster"

#define MAX_FINGERPRINTS 5
#define FINGERPRINT_MAX_COUNT 5

#define BUFFER_SIZE 64
#define AUTH_RESULT_LENGTH 69
#define TZ_RESPONSE_OFFSET 256

typedef int32_t err_t;

typedef struct {
    uint32_t print_count;
    uint32_t prints[MAX_FINGERPRINTS];
} fpc_fingerprint_index_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint32_t ret_val;  // some cases this is used for return value of the command
} fpc_send_std_cmd_t;

typedef struct {
    uint32_t cmd_id;
    uint32_t ret_val;  // some cases this is used for return value of the command
    uint32_t length;   // some length of data supplied by previous modified command
} keymaster_cmd_t;

typedef struct {
    int32_t status;
    uint32_t offset;
    uint32_t length;
} keymaster_return_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint64_t challenge;
    int32_t status;
} fpc_send_auth_cmd_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint32_t gid;
    int32_t status;
} fpc_set_gid_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    int32_t status;
    uint32_t length;
    char data[];
} fpc_send_keydata_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint64_t challenge;
    int32_t status;
} fpc_load_auth_challenge_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    int32_t status;
    uint32_t remaining_touches;
} fpc_enrol_step_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint32_t print_id;
    int32_t status;
} fpc_end_enrol_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    int32_t status;
    uint32_t length;
    char* data;
} fpc_send_buffer_t;

typedef struct {
    uint32_t commandgroup;
    uint32_t command;
    int32_t status;
    uint32_t id;
    uint32_t dbg1;
    uint32_t dbg2;
} fpc_send_identify_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    int32_t status;
    uint32_t length;
    uint32_t fingerprints[FINGERPRINT_MAX_COUNT];
} fpc_fingerprint_list_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint32_t fingerprint_id;
    int32_t status;
} fpc_fingerprint_delete_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint32_t result;
    uint32_t length;
    uint8_t auth_result[AUTH_RESULT_LENGTH];  // In practice this is always 69 bytes
} fpc_get_auth_result_t;

typedef struct {
    uint32_t length;  // Length of data on ion buffer
    uint32_t v_addr;  // Virtual address of ion mmap buffer
} fpc_send_mod_cmd_t;

typedef struct {
    uint32_t group_id;
    uint32_t cmd_id;
    uint64_t auth_id;
} fpc_get_db_id_cmd_t;

typedef struct {
    int epoll_fd;
    int dev_fd;
    int event_fd;
} fpc_event_t;

typedef struct fpc_imp_data_t {
    fpc_event_t event;
} fpc_imp_data_t;

enum {
    FPC_PWROFF = 0,
    FPC_PWRON = 1,
};

enum {
    FPC_EVENT_FINGER,
    FPC_EVENT_EVENTFD,
    FPC_EVENT_TIMEOUT,
    FPC_EVENT_ERROR,
};

enum fingerprint_group_t {
    FPC_GROUP_NORMAL = 0x1,
    FPC_GROUP_DB = 0x2,
    FPC_GROUP_FPCDATA = 0x3,
    FPC_GROUP_DEBUG = 0x6,
    FPC_GROUP_QC = 0x07,
};

enum fingerprint_normal_cmd_t {
    FPC_BEGIN_ENROL = 0x00,
    FPC_ENROL_STEP = 0x01,
    FPC_END_ENROL = 0x02,
    FPC_IDENTIFY = 0x03,
    FPC_UPDATE_TEMPLATE = 0x04,
    FPC_WAIT_FINGER_LOST = 0x05,
    FPC_WAIT_FINGER_DOWN = 0x07,
    FPC_GET_FINGER_STATUS = 0x8,
    FPC_LOAD_EMPTY_DB = 0x0A,
    FPC_GET_FINGERPRINTS = 0xD,
    FPC_DELETE_FINGERPRINT = 0xE,
    FPC_CAPTURE_IMAGE = 0xF,
    FPC_SET_GID = 0x10,
    FPC_GET_TEMPLATE_ID = 0x11,
    FPC_INIT = 0x12,
};

enum fingerprint_fpcdata_cmd_t {
    FPC_SET_AUTH_CHALLENGE = 0x01,
    FPC_GET_AUTH_CHALLENGE = 0x02,
    FPC_AUTHORIZE_ENROL = 0x03,
    FPC_GET_AUTH_RESULT = 0x04,
    FPC_SET_KEY_DATA = 0x05,
    FPC_IS_USER_VALID = 0x07,
};

enum fingerprint_db_cmd_t {
    FPC_LOAD_DB = 0x0B,
    FPC_STORE_DB = 0x0C,
};

enum fingerprint_debug_cmd_t {
    FPC_GET_SENSOR_INFO = 0x03,
};

enum fingerprint_qc_cmd_t {
    FPC_SET_QC_AUTH_NONCE = 0x01,
    FPC_GET_QC_AUTH_RESULT = 0x02,
};

uint64_t fpc_load_db_id(fpc_imp_data_t* data);
uint64_t fpc_load_auth_challenge(fpc_imp_data_t* data);

err_t fpc_auth_end(fpc_imp_data_t* data);
err_t fpc_auth_start(fpc_imp_data_t* data);
err_t fpc_auth_step(fpc_imp_data_t* data, uint32_t* print_id);
err_t fpc_capture_image(fpc_imp_data_t* data);
err_t fpc_close(fpc_imp_data_t** data);
err_t fpc_del_print_id(fpc_imp_data_t* data, uint32_t id);
err_t fpc_enroll_end(fpc_imp_data_t* data, uint32_t* print_id);
err_t fpc_enroll_start(fpc_imp_data_t* data, int print_index);
err_t fpc_enroll_step(fpc_imp_data_t* data, uint32_t* remaining_touches);
err_t fpc_event_create(fpc_event_t*, int event_fd);
err_t fpc_event_destroy(fpc_event_t*);
err_t fpc_get_hw_auth_obj(fpc_imp_data_t* data, void* buffer, uint32_t length);
err_t fpc_get_power(const fpc_event_t*);
err_t fpc_get_print_index(fpc_imp_data_t* data, fpc_fingerprint_index_t* idx_data);
err_t fpc_get_user_db_length(fpc_imp_data_t* data);
err_t fpc_init(fpc_imp_data_t** data, int event_fd);
err_t fpc_load_empty_db(fpc_imp_data_t* data);
err_t fpc_load_user_db(fpc_imp_data_t* data, char* path);
err_t fpc_poll_event(const fpc_event_t*);
err_t fpc_set_auth_challenge(fpc_imp_data_t* data, uint64_t challenge);
err_t fpc_set_gid(fpc_imp_data_t* data, uint32_t gid);
err_t fpc_set_power(const fpc_event_t*, int poweron);
err_t fpc_store_user_db(fpc_imp_data_t* data, uint32_t length, char* path);
err_t fpc_update_template(fpc_imp_data_t* data);
err_t fpc_verify_auth_challenge(fpc_imp_data_t* data, void* hat, uint32_t size);
err_t fpc_wait_finger_down(fpc_imp_data_t* data);
err_t fpc_wait_finger_lost(fpc_imp_data_t* data);

#ifdef __cplusplus
}
#endif

#endif  // __FINGERPRINTSCANNER_H_
