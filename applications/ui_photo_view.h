/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_UI_PHOTO_VIEW_H_
#define APPLICATIONS_UI_PHOTO_VIEW_H_

#include "lvgl.h"
#include <rtthread.h>

lv_obj_t* ui_photo_view_create();

void ui_photo_view_destroy(lv_obj_t*);

int get_switch_interval_config();

#endif // APPLICATIONS_UI_PHOTO_VIEW_H_