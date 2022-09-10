/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "image_node.h"
#include "common_utils.h"

#ifndef DBG_LVL
#define DBG_LVL DBG_LOG
#endif
#include <rtdbg.h>

#include <string.h>

image_node_t* image_node_create(const char* path)
{
    if (path == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_NULL;
    }

    LOG_I("%s: path=%s", __func__, path);
    const char* ext = path_get_ext(path);
    return image_node_create2(path, ext);
}

image_node_t* image_node_create2(const char* path, const char* format)
{
    if (path == RT_NULL) {
        LOG_E("%s: invalid arguments!", __func__);
        return RT_NULL;
    }

    image_node_t* image_node = rt_calloc(sizeof(image_node_t), 1);
    if (image_node == RT_NULL) {
        return RT_NULL;
    }

    image_node->path = rt_strdup(path);
    image_node->format = rt_strdup(format);
    image_node->node.next = RT_NULL;
    image_node->node.prev = RT_NULL;

    LOG_I("%s: image_node %p (%s, %s) create success!", __func__, image_node, path, format);
    return image_node;
}

image_node_t* image_node_copy(image_node_t* node)
{
    if (node == RT_NULL) {
        return RT_NULL;
    }

    return image_node_create2(node->path, node->format);
}

void image_node_destroy(image_node_t* node)
{
    if (node == RT_NULL) {
        return;
    }

    rt_free((void*)node->path);
    rt_free((void*)node->format);
    rt_free((void*)node);
    LOG_I("%s: image_node %p destroy success!", __func__, node);
}

void image_node_dump(image_node_t* node)
{
    if (node == RT_NULL) {
        return;
    }

    LOG_I("image_node %p, path: %s, format: %s", node, node->path, node->format);
}

void test_image_node()
{
    image_node_t* image_node = image_node_create("/mnt/sd0/1.png");
    rt_kprintf("image_node path: %s\n", image_node->path);
    rt_kprintf("image_node format: %s\n", image_node->format);
    rt_kprintf("image_node is image: %d\n", path_is_image(image_node->path));

    image_node_t* copy_node = image_node_copy(image_node);
    rt_kprintf("copy_node path: %s\n", copy_node->path);
    rt_kprintf("copy_node format: %s\n", copy_node->format);
    rt_kprintf("copy_node is image: %d\n", path_is_image(copy_node->path));

    image_node_destroy(copy_node);
    image_node_destroy(image_node);
}
MSH_CMD_EXPORT(test_image_node, image_node unit test);
