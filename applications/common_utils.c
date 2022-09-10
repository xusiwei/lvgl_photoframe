/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "common_utils.h"

#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_size_t list_move(rt_list_t* dst_head, rt_list_t* src_head)
{
    rt_list_t *p = RT_NULL, *n = RT_NULL;
    rt_size_t count = 0;

    if (src_head == RT_NULL || dst_head == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return 0;
    }

    rt_list_init(dst_head);
    rt_list_for_each_safe(p, n, src_head)
    {
        LOG_D("move node %p to list %p ...", p, dst_head);
        rt_list_insert_before(dst_head, p);
        count++;
    }
    return count;
}

rt_size_t list_sort(rt_list_t* head, int (*comp)(rt_list_t*, rt_list_t*))
{
    rt_list_t* p = RT_NULL;
    rt_list_t sorted_head = {0};
    rt_size_t count = 0;

    if (head == RT_NULL || comp == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return 0;
    }

    // rt_list_len(head) <= 1
    if (head->next == RT_NULL || head->next->next == RT_NULL) {
        return 0;
    }

    rt_list_init(&sorted_head);
    while (head->next) {
        // select the minial item on list
        rt_list_t* pmin = head->next;
        if (head->next->next) {
            rt_list_for_each(p, head->next)
            {
                if (comp(p, pmin) < 0) {
                    pmin = p;
                }
            }
        }

        // append to tail of sorted list
        if (pmin) {
            rt_list_insert_before(&sorted_head, pmin);
            count++;
        }
    }

    // update head
    *head = sorted_head;
    LOG_I("%s: %u node sorted!", __func__, count);
    return count;
}

void list_destroy(rt_list_t* head, void (*destroy)(rt_list_t*))
{
    rt_list_t* p = RT_NULL;
    rt_list_t* n = RT_NULL;
    unsigned count = 0;

    if (head == RT_NULL || destroy == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return;
    }

    if (head->next == RT_NULL) {
        LOG_I("%s: empty list!", __func__);
        return;
    }

    rt_list_for_each_safe(p, n, head)
    {
        destroy(p);
        count++;
    }

    LOG_I("%s: %d node destroyed!", __func__, count);
}

const char* str_rfind(const char* str, char c)
{
    if (str == RT_NULL) {
        return RT_NULL;
    }

    int len = (int)rt_strlen(str);
    for (int i = len - 1; i >= 0; i--) {
        if (str[i] == c) {
            return &str[i];
        }
    }
    return RT_NULL;
}

const char* path_get_ext(const char* path)
{
    const char* dot = str_rfind(path, '.');
    if (dot) {
        return dot + 1;
    }
    return RT_NULL;
}

rt_bool_t path_is_image(const char* path)
{
    if (path == RT_NULL) {
        return RT_FALSE;
    }

    const char* ext = path_get_ext(path);
    return (rt_strcmp(ext, "jpg") == 0 || rt_strcmp(ext, "png") == 0) ? RT_TRUE : RT_FALSE;
}

const char* path_join(const char* dir, const char* file, unsigned file_len)
{
    if (dir == RT_NULL || file == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_NULL;
    }

    unsigned dir_len = rt_strlen(dir);
    unsigned path_len = dir_len + 1 + file_len + 1;

    char* path = rt_malloc(path_len);
    if (path == RT_NULL) {
        LOG_E("%s: malloc %d failed!", __func__, path_len);
        return RT_NULL;
    }

    rt_strncpy(path, dir, dir_len);
    path[dir_len] = '/';
    rt_strncpy(&path[dir_len + 1], file, file_len);
    path[path_len - 1] = '\0';
    return path;
}
