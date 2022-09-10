/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_COMMON_UTILS_H_
#define APPLICATIONS_COMMON_UTILS_H_

#include <rtthread.h>

rt_size_t list_move(rt_list_t* dst_head, rt_list_t* src_head);

rt_size_t list_sort(rt_list_t* head, int (*comp)(rt_list_t*, rt_list_t*));

void list_destroy(rt_list_t* head, void (*destroy)(rt_list_t*));

const char* path_get_ext(const char* path);

rt_bool_t path_is_image(const char* path);

const char* path_join(const char* dir, const char* file, unsigned file_len);

#endif // APPLICATIONS_COMMON_UTILS_H_