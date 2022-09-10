/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "ui_config_view.h"
#include "kv_config.h"

#ifndef DBG_LVL
#define DBG_LVL DBG_LOG
#endif
#include <rtdbg.h>

static lv_obj_t* slider_label;

static void slider_event_cb(lv_event_t* e)
{
    static char buf[4];
    lv_obj_t* slider = lv_event_get_target(e);

    rt_snprintf(buf, 4, "%u", lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    LOG_I("prepare to update config %s=%s", CFG_SWITCH_INTERVAL, buf);
    if (!kv_config_set(CFG_SWITCH_INTERVAL, buf)) {
        LOG_E("update config %s=%s failed!", CFG_SWITCH_INTERVAL, buf);
    }
}

extern int get_switch_interval_config();

lv_obj_t* ui_config_view_create()
{
    LOG_I("ui_config_view_create: create container ...");
    // create a container
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_center(cont);

    LOG_I("ui_config_view_create: create slider ...");
    // Create a slider in the center of the display
    lv_obj_t* slider = lv_slider_create(cont);
    lv_obj_center(slider);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_range(slider, 1, 100);

    // Create a label below the slider
    LOG_I("ui_config_view_create: create slider label ...");
    slider_label = lv_label_create(cont);
    lv_label_set_text(slider_label, "0");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // Create an informative label
    LOG_I("ui_config_view_create: create info label ...");
    lv_obj_t* info_label = lv_label_create(cont);
    lv_label_set_text(info_label, "Set the photo switch interval:");
    lv_obj_align_to(info_label, slider, LV_ALIGN_OUT_TOP_MID, 10, -10);
    return cont;
}

void ui_config_view_destroy(lv_obj_t* obj)
{
    if (obj) {
        lv_obj_del(obj);
    }
}
