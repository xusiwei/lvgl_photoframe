/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#ifndef APPLICATIONS_IMAGE_NODE_H_
#define APPLICATIONS_IMAGE_NODE_H_

#include <rtthread.h>

struct image_node {
    rt_list_t node;
    const char* path;
    const char* format;
};

typedef struct image_node image_node_t;

image_node_t* image_node_create(const char* path);

image_node_t* image_node_create2(const char* path, const char* format);

image_node_t* image_node_copy(image_node_t* node);

void image_node_destroy(image_node_t* node);

void image_node_dump(image_node_t* node);

const char* str_rfind(const char* str, char c);

#endif // APPLICATIONS_IMAGE_NODE_H_
