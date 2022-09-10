/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "image_scaner.h"
#include "common_macros.h"
#include "common_utils.h"
#include "image_node.h"

#define DBG_LVL DBG_LOG
#include <dirent.h>
#include <rtdbg.h>

#define SCANER_STACK_SIZE 4096
#define SCANER_STACK_PRIO 10
#define SCANER_STACK_TICK 10

static void scaner_scan(image_scaner_t scaner, const char* cur_path)
{
    DIR* dir = opendir(cur_path);
    if (dir == RT_NULL || cur_path == RT_NULL) {
        return;
    }

    struct dirent* ent = RT_NULL;
    while ((ent = readdir(dir)) != RT_NULL) {
        if (rt_strcmp(ent->d_name, ".") == 0 || rt_strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        const char* sub_path = path_join(cur_path, ent->d_name, ent->d_namlen);
        if (sub_path == RT_NULL) {
            LOG_E("%s: join path %s/%s failed!", __func__, cur_path, ent->d_name);
            break;
        }

        LOG_I("sub_path=%s", sub_path);
        if (ent->d_type == DT_DIR) {
            scaner_scan(scaner, sub_path);
        } else if (ent->d_type == DT_REG) {
            if (path_is_image(sub_path)) {
                LOG_I("found image: %s", sub_path);
                image_node_t* img_node = image_node_create(sub_path);
                if (img_node != RT_NULL) {
                    rt_list_insert_before(&scaner->scaning_list, &img_node->node);
                    LOG_I("%s: add image_node %s success!", __func__, img_node->path);
                } else {
                    LOG_E("%s: create image_node failed!", __func__);
                }
            }
        } else {
            LOG_W("%s: unknow file entry %s!", __func__, sub_path);
        }
        rt_free((void*)sub_path);
    }

    closedir(dir);
}

static void scaner_work(void* param)
{
    image_scaner_t scaner = (image_scaner_t)param;

    if (scaner == RT_NULL) {
        LOG_E("%s: invalid param!", __func__);
        return;
    }

    LOG_I("%s: thread start!", __func__);
    while (scaner->running) {
        // wait for trigger scan
        RT_CHECK(rt_sem_take(scaner->trig_sem, RT_WAITING_FOREVER));
        if (!scaner->running) {
            break;
        }

        // traversal all sub dir, add image nodes to scaning_list
        LOG_I("start scanning...");
        rt_list_init(&scaner->scaning_list);
        scaner_scan(scaner, scaner->top_dir);

        // update image_list with scaning_list
        // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
        RT_CHECK(rt_mutex_take(scaner->mutex, RT_WAITING_FOREVER));
        rt_size_t count = list_move(&scaner->image_list, &scaner->scaning_list);
        RT_CHECK(rt_mutex_release(scaner->mutex));
        LOG_I("scanned image: %d", count);

        // notify scan done!
        RT_CHECK(rt_sem_release(scaner->done_sem));
        LOG_I("once scan work done...");
    }

    LOG_I("%s: thread exit!", __func__);
    RT_CHECK(rt_sem_release(scaner->stop_sem));
}

image_scaner_t image_scaner_create(const char* top_dir)
{
    if (top_dir == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_NULL;
    }

    image_scaner_t scaner = rt_calloc(sizeof(struct image_scaner), 1);
    if (scaner == RT_NULL) {
        LOG_E("%s: alloc image_scaner failed!", __func__);
        return RT_NULL;
    }

    rt_thread_t thread =
        rt_thread_create("scaner", scaner_work, scaner, SCANER_STACK_SIZE, SCANER_STACK_PRIO, SCANER_STACK_TICK);
    if (thread == RT_NULL) {
        LOG_E("%s: create thread failed!", __func__);
        rt_free(scaner);
        return RT_NULL;
    }

    rt_mutex_t mutex = rt_mutex_create("IS_mtx", RT_IPC_FLAG_FIFO);
    if (mutex == RT_NULL) {
        LOG_E("%s: create mutex failed!", __func__);
        rt_free(scaner);
        rt_thread_delete(thread);
        return RT_NULL;
    }

    rt_sem_t trig_sem = rt_sem_create("IS_trig_sem", 0, RT_IPC_FLAG_FIFO);
    if (trig_sem == RT_NULL) {
        LOG_E("%s: create trig_sem failed!", __func__);
        rt_free(scaner);
        rt_thread_delete(thread);
        rt_mutex_delete(mutex);
    }

    rt_sem_t done_sem = rt_sem_create("IS_done_sem", 0, RT_IPC_FLAG_FIFO);
    if (done_sem == RT_NULL) {
        LOG_E("%s: create done_sem failed!", __func__);
        rt_free(scaner);
        rt_thread_delete(thread);
        rt_mutex_delete(mutex);
        rt_sem_delete(trig_sem);
    }

    rt_sem_t stop_sem = rt_sem_create("IS_stop_sem", 0, RT_IPC_FLAG_FIFO);
    if (stop_sem == RT_NULL) {
        LOG_E("%s: create stop_sem failed!", __func__);
        rt_free(scaner);
        rt_mutex_delete(mutex);
        rt_thread_delete(thread);
        rt_sem_delete(trig_sem);
        rt_sem_delete(done_sem);
    }

    rt_list_init(&scaner->image_list);
    rt_list_init(&scaner->scaning_list);

    scaner->running = RT_TRUE;
    scaner->top_dir = rt_strdup(top_dir);
    scaner->thread = thread;
    scaner->mutex = mutex;
    scaner->trig_sem = trig_sem;
    scaner->stop_sem = stop_sem;
    scaner->done_sem = done_sem;

    LOG_I("%s: image_scaner create success!", __func__);
    return scaner;
}

void image_scaner_destroy(image_scaner_t scaner)
{
    if (scaner == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return;
    }

    if (scaner->running) {
        image_scaner_stop(scaner);
    }
    rt_sem_delete(scaner->stop_sem);
    rt_sem_delete(scaner->done_sem);
    rt_sem_delete(scaner->trig_sem);
    rt_mutex_delete(scaner->mutex);
    rt_thread_delete(scaner->thread);
    rt_free((void*)scaner->top_dir);
    rt_free(scaner);
    LOG_I("%s: image_scaner destroy success!", __func__);
}

rt_bool_t image_scaner_start(image_scaner_t scaner)
{
    if (scaner == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_FALSE;
    }

    RT_CHECK_RETURN(rt_thread_startup(scaner->thread), RT_FALSE);
    return RT_TRUE;
}

rt_bool_t image_scaner_stop(image_scaner_t scaner)
{
    if (scaner == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_FALSE;
    }

    scaner->running = RT_FALSE;
    RT_CHECK_RETURN(rt_sem_release(scaner->trig_sem), RT_FALSE);
    RT_CHECK_RETURN(rt_sem_take(scaner->stop_sem, RT_WAITING_FOREVER), RT_FALSE);
    return RT_TRUE;
}

rt_bool_t image_scaner_trigger_scan(image_scaner_t scaner)
{
    if (scaner == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_NULL;
    }

    RT_CHECK_RETURN(rt_sem_release(scaner->trig_sem), RT_FALSE);
    return RT_TRUE;
}

rt_size_t image_scaner_get_images(image_scaner_t scaner, rt_list_t* img_list)
{
    if (scaner == RT_NULL || img_list == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return 0;
    }

    LOG_I("take done_sem ...");
    RT_CHECK_RETURN(rt_sem_take(scaner->done_sem, RT_WAITING_FOREVER), 0);
    LOG_I("after take done_sem ...");

    rt_size_t count = 0;
    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    RT_CHECK(rt_mutex_take(scaner->mutex, RT_WAITING_FOREVER));

    count = list_move(img_list, &scaner->image_list);
    LOG_D("scanned images: %d", count);

    RT_CHECK(rt_mutex_release(scaner->mutex));
    LOG_I("release mutex ...");
    return count;
}

static void test_image_scaner()
{
    image_scaner_t scaner = image_scaner_create("/mnt/sd0");

    if (!image_scaner_start(scaner)) {
        LOG_I("image_scaner start failed!");
    }
    LOG_I("image_scaner start success!");

    if (!image_scaner_trigger_scan(scaner)) {
        LOG_I("image_scaner trigger failed!");
    }
    LOG_I("image_scaner trigger success!");

    rt_list_t img_list;
    rt_size_t count = image_scaner_get_images(scaner, &img_list);
    LOG_I("image_scaner scanned %u images !", count);

    rt_list_t *p = RT_NULL, *n = RT_NULL;
    rt_list_for_each_safe(p, n, &img_list)
    {
        image_node_t* node = rt_container_of(p, image_node_t, node);
        image_node_dump(node);
        image_node_destroy(node);
    }

    if (!image_scaner_stop(scaner)) {
        LOG_I("image_scaner stop failed!");
    }
    LOG_I("image_scaner stop success!");
    image_scaner_destroy(scaner);
    LOG_I("%s: done!", __func__);
}
MSH_CMD_EXPORT(test_image_scaner, image_scaner unit test);
