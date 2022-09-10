/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "ui_photo_view.h"

#ifndef DBG_LVL
#define DBG_LVL DBG_LOG
#endif
#include <rtdbg.h>
#include <stdlib.h>

#include "image_node.h"
#include "image_scaner.h"
#include "kv_config.h"
#include "lvgl.h"
#include "ui_thread_init.h"

static char image_path[256];

static lv_timer_t* switch_timer;

static rt_list_t image_list_head;

struct timer_data {
    lv_obj_t* img;
    rt_list_t* list;
    rt_list_t* iter;
};

int get_switch_interval_config()
{
    char switch_seconds[16] = CFG_SWITCH_INTERVAL_DEFAULT;

    kv_config_get(CFG_SWITCH_INTERVAL, switch_seconds, sizeof(switch_seconds));
    if (rt_strlen(switch_seconds) == 0) {
        rt_strcpy(switch_seconds, CFG_SWITCH_INTERVAL_DEFAULT);
        kv_config_set(CFG_SWITCH_INTERVAL, switch_seconds);
        LOG_I("%s: set config %s=%s", __func__, CFG_SWITCH_INTERVAL, switch_seconds);
    } else {
        LOG_I("%s: got config %s=%s", __func__, CFG_SWITCH_INTERVAL, switch_seconds);
    }
    return atoi(switch_seconds) * 1000;
}

void timer_cb(lv_timer_t* timer)
{
    struct timer_data* ctx = (struct timer_data*)timer->user_data;
    RT_ASSERT(ctx->img);
    RT_ASSERT(ctx->list);
    RT_ASSERT(ctx->iter);

    image_node_t* image_node = rt_container_of(ctx->iter, image_node_t, node);
    LOG_I("iter = %p, %s", ctx->iter, image_node->path);

    rt_snprintf(image_path, sizeof(image_path), "A:%s", image_node->path);
    LOG_I("update_src: %s", image_path);
    lv_img_set_src(ctx->img, image_path);

    // update for next show image
    ctx->iter = ctx->iter->next;  // move to next;
    if (ctx->iter == ctx->list) { // handle for tail node
        ctx->iter = ctx->list->next;
    }

    // update timer period
    int switch_ms = get_switch_interval_config();
    LOG_I("switch_ms = %d", switch_ms);
    lv_timer_set_period(timer, switch_ms);
}

rt_bool_t get_image_list(rt_list_t* image_list)
{
    image_scaner_t scaner = image_scaner_create("/mnt/sd0");
    if (!scaner) {
        LOG_I("%s: image_scaner create failed!", __FUNCTION__);
        return RT_FALSE;
    }

    LOG_I("%s: image_scaner create success!", __FUNCTION__);
    rt_bool_t retval = image_scaner_start(scaner);
    if (!retval) {
        LOG_I("%s: image_scaner start failed!", __FUNCTION__);
        image_scaner_destroy(scaner);
        return RT_FALSE;
    }
    LOG_I("%s: image_scaner start success!", __FUNCTION__);

    retval = image_scaner_trigger_scan(scaner);
    if (!retval) {
        LOG_I("%s: image_scaner trigger scan failed!", __FUNCTION__);
        image_scaner_destroy(scaner);
        return RT_FALSE;
    }
    LOG_I("%s: image_scaner trigger scan success!", __FUNCTION__);

    rt_size_t count = image_scaner_get_images(scaner, image_list);
    LOG_I("%s: image_scaner found %d images!", __FUNCTION__, count);

    return RT_TRUE;
}

lv_obj_t* ui_photo_view_create()
{
    // wait sdcard mount success
    ui_wait_fs_ready();

    // get image list from sdcard
    rt_list_t* image_list = RT_NULL;
    if (get_image_list(&image_list_head)) {
        image_list = &image_list_head;
    }

    // create a container
    lv_obj_t* cont = lv_obj_create(NULL); // lv_scr_act()
    lv_obj_center(cont);

    // create image object
    lv_obj_t* img = lv_img_create(cont);
    lv_obj_center(img);

    // set image src
    if (image_list) {
        rt_list_t* node = image_list->next;
        if (node) {
            image_node_t* image_node = rt_container_of(node, image_node_t, node);
            rt_snprintf(image_path, sizeof(image_path), "A:%s", image_node->path);
            lv_img_set_src(img, image_path);
            LOG_I("%s: set_src %s", __func__, image_path);

            rt_list_t* p;
            rt_list_for_each(p, image_list)
            {
                image_node_t* node = rt_container_of(p, image_node_t, node);
                LOG_I("node: %s", node->path);
            }

            static struct timer_data context;
            context.img = img;
            context.list = image_list;
            context.iter = node->next;
            LOG_I("iter = %p, %s", node, image_node->path);

            int switch_ms = get_switch_interval_config();
            LOG_I("switch_ms = %d", switch_ms);
            switch_timer = lv_timer_create(timer_cb, switch_ms, &context);
        }
    }

    return cont;
}

void ui_photo_view_destroy(lv_obj_t* obj)
{
    if (obj) {
        lv_obj_del(obj);
    }
    if (switch_timer) {
        lv_timer_del(switch_timer);
    }
}
