/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_KV_CONFIG_H_
#define APPLICATIONS_KV_CONFIG_H_

#include <rtthread.h>

rt_bool_t kv_config_init();

rt_bool_t kv_config_set(const char* key, const char* value);

rt_bool_t kv_config_get(const char* key, char* value, rt_size_t val_len);

#define CFG_SWITCH_INTERVAL "switch_interval"
#define CFG_SWITCH_INTERVAL_DEFAULT "10"

#endif // APPLICATIONS_KV_CONFIG_H_