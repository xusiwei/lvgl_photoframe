/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_IMAGE_SCANER_H_
#define APPLICATIONS_IMAGE_SCANER_H_

#include <rtthread.h>

struct image_scaner {
    rt_list_t image_list;
    rt_list_t scaning_list;
    rt_bool_t running;
    const char* top_dir;

    rt_thread_t thread; // worker thread
    rt_mutex_t mutex;   // for protect lists
    rt_sem_t trig_sem;  // for trigger worker thread start scan
    rt_sem_t done_sem;  // for notify worker thread scan done
    rt_sem_t stop_sem;  // for notify others worker thread stoped
};

typedef struct image_scaner* image_scaner_t;

image_scaner_t image_scaner_create(const char* top_dir);

void image_scaner_destroy(image_scaner_t scaner);

rt_bool_t image_scaner_start(image_scaner_t scaner);

rt_bool_t image_scaner_stop(image_scaner_t scaner);

rt_bool_t image_scaner_trigger_scan(image_scaner_t scaner);

rt_size_t image_scaner_get_images(image_scaner_t scaner, rt_list_t* img_list);

#endif /* APPLICATIONS_IMAGE_SCANER_H_ */
