/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_UI_THREAD_INIT_H_
#define APPLICATIONS_UI_THREAD_INIT_H_

#include "lvgl.h"
#include <rtthread.h>

void ui_set_current(lv_obj_t* obj);

lv_obj_t* ui_get_current();

void ui_wait_fs_ready();

enum {
    KEY_K1 = 1,
    KEY_K2,
    KEY_K3,
    KEY_K4,
    KEY_K5,
    KEY_K6,
};

enum {
    K_RELASED,
    K_PRESSED,
};

struct ui_key_msg {
    uint8_t key;
    uint8_t state;
};

typedef struct ui_key_msg ui_key_msg_t;

rt_mq_t ui_get_key_mq();

#endif // APPLICATIONS_UI_THREAD_INIT_H_