/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_UI_CONFIG_VIEW_H_
#define APPLICATIONS_UI_CONFIG_VIEW_H_

#include "lvgl.h"
#include <rtthread.h>

lv_obj_t* ui_config_view_create();

void ui_config_view_destroy(lv_obj_t*);

#endif // APPLICATIONS_UI_CONFIG_VIEW_H_