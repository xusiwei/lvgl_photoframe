/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-17     Meco Man      First version
 */
#include <lvgl.h>
#include <rtthread.h>
#define DBG_TAG "LVGL"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include "kv_config.h"
#include "ui_config_view.h"
#include "ui_photo_view.h"
#include "ui_thread_init.h"

#define LV_THREAD_STACK_SIZE 10240
#define LV_THREAD_PRIO 20
#define LV_THREAD_TICK 10

struct rt_mutex ui_mutex;

lv_obj_t* ui_current_view = NULL;

static rt_bool_t ui_key_mq_inited = RT_FALSE;
static struct rt_messagequeue ui_key_mq;
static rt_uint8_t ui_key_msgpool[8192];

void ui_set_current(lv_obj_t* obj)
{
    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    rt_mutex_take(&ui_mutex, RT_WAITING_FOREVER);
    ui_current_view = obj;
    rt_mutex_release(&ui_mutex);
}

lv_obj_t* ui_get_current()
{
    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    rt_mutex_take(&ui_mutex, RT_WAITING_FOREVER);
    lv_obj_t* obj = ui_current_view;
    rt_mutex_release(&ui_mutex);
    return obj;
}

rt_mq_t ui_get_key_mq()
{
    if (!ui_key_mq_inited) {
        return RT_NULL;
    }

    return &ui_key_mq;
}

void ui_wait_fs_ready()
{
    static rt_bool_t fs_ready = RT_FALSE;
    if (fs_ready) {
        LOG_I("%s: fs already available!", __func__);
        return;
    }

    LOG_I("%s: waiting for fs ready...", __func__);
    extern struct rt_semaphore sd_fs_sem;
    rt_sem_take(&sd_fs_sem, RT_WAITING_FOREVER);
    fs_ready = RT_TRUE;
}

static void lv_work(void* parameter)
{
    rt_bool_t kv_init = kv_config_init();
    rt_kprintf("kv_config_init: %s", kv_init ? "OK" : "FAIL");

    lv_obj_t* photo_view = ui_photo_view_create();
    ui_set_current(photo_view);

    lv_obj_t* config_view = ui_config_view_create();

    lv_obj_t* last_ui = RT_NULL;
    while (1) {
        // load switched ui
        lv_obj_t* current_ui = ui_get_current();
        if (current_ui != last_ui) {
            lv_scr_load(current_ui);
            last_ui = current_ui;
        }

        // handle LVGL tick
        lv_task_handler();
        rt_thread_mdelay(1);

        // handle keypad event
        ui_key_msg_t msg = {0};
        if (rt_mq_recv(&ui_key_mq, &msg, sizeof(msg), RT_WAITING_NO) == RT_EOK) {
            LOG_I("got key msg: %d %d", msg.key, msg.state);
            // use K1, K2 to switch config_view, photo_view
            if (msg.key == KEY_K1) {
                ui_set_current(config_view);
            } else if (msg.key == KEY_K2) {
                ui_set_current(photo_view);
            }
        }
    }
}

int ui_thread_init(void)
{
    rt_thread_t tid;

    rt_err_t err = rt_mutex_init(&ui_mutex, "ui_mtx", RT_IPC_FLAG_FIFO);
    RT_ASSERT(err == RT_EOK);

    err = rt_mq_init(&ui_key_mq, "key_mq", ui_key_msgpool, // message pool
                     sizeof(ui_key_msg_t), sizeof(ui_key_msgpool), RT_IPC_FLAG_FIFO);
    RT_ASSERT(err == RT_EOK);
    ui_key_mq_inited = RT_TRUE;

    tid = rt_thread_create("UI", lv_work, RT_NULL, LV_THREAD_STACK_SIZE, LV_THREAD_PRIO, LV_THREAD_TICK);
    if (tid == RT_NULL) {
        LOG_E("create 'UI' thread failed");
        return -1;
    }
    rt_thread_startup(tid);

    return 0;
}
INIT_APP_EXPORT(ui_thread_init);
