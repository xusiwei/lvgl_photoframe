/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-18     Meco Man      First version
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include "rtconfig.h"

#define LV_USE_GPU_N9H30_GE2D 1
// #define LV_USE_ANTI_TEARING             1

#define LV_COLOR_DEPTH BSP_LCD_BPP
#define LV_HOR_RES_MAX BSP_LCD_WIDTH
#define LV_VER_RES_MAX BSP_LCD_HEIGHT

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_USE_PERF_MONITOR 1
// #define LV_DISP_DEF_REFR_PERIOD         16
//  #define CONFIG_LV_LOG_LEVEL LV_LOG_LEVEL_TRACE

#ifndef BIT31
#define BIT31 (0x80000000) ///< Bit 31 mask of an 32 bit integer
#endif

#define IS_CACHEABLE_VRAM(addr) !((uint32_t)addr & BIT31)

#define LV_VERSION_EQUAL(x, y, z) (x == LVGL_VERSION_MAJOR && y == LVGL_VERSION_MINOR && z == LVGL_VERSION_PATCH)

#ifdef PKG_USING_LV_MUSIC_DEMO
#define LV_USE_DEMO_RTT_MUSIC 1
#endif

#if LV_USE_DEMO_RTT_MUSIC
#define LV_DEMO_RTT_MUSIC_AUTO_PLAY 1

#define LV_USE_DEMO_MUSIC 1
#define LV_DEMO_MUSIC_AUTO_PLAY 1
#endif

/* Please comment LV_USE_DEMO_RTT_MUSIC declaration before un-comment below */
#if defined(PKG_LVGL_USING_DEMOS)
#define LV_USE_DEMO_WIDGETS 1
#endif

// #define LV_USE_DEMO_BENCHMARK       1

#define LV_USE_FS_POSIX 1
#define LV_FS_POSIX_LETTER 'A'

#define LV_USE_GIF 1
#define LV_USE_PNG 1
#define LV_USE_SJPG 1

#define LV_IMG_CACHE_DEF_SIZE 16

#endif
