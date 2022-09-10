/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-21     xu           the first version
 */
#include "kv_config.h"

#include <flashdb.h>

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <string.h>

#define CONFIG_DB_NAME "config"
#define CONFIG_DB_PATH "/mnt/fminand/cfg"
#define CONFIG_DB_SECSIZE 65536
#define CONFIG_DB_SIZE 1048576
// kvdb probe config  /mnt/fminand/cfg 65536 1048576

static struct rt_mutex config_mtx;
static struct fdb_kvdb config_db = {0};

static rt_bool_t inited = RT_FALSE;

rt_bool_t kv_config_init_locked()
{
    rt_bool_t not_formatable = RT_FALSE;
    rt_bool_t file_mode = RT_TRUE;
    rt_size_t sec_size = CONFIG_DB_SECSIZE;
    rt_size_t db_size = CONFIG_DB_SIZE;

    memset(&config_db, 0, sizeof(config_db));

    extern void ui_wait_fs_ready();
    ui_wait_fs_ready();

    fdb_kvdb_control(&config_db, FDB_KVDB_CTRL_SET_NOT_FORMAT, &not_formatable);
    fdb_kvdb_control(&config_db, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
    fdb_kvdb_control(&config_db, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
    fdb_kvdb_control(&config_db, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);

    rt_err_t err = fdb_kvdb_init(&config_db, CONFIG_DB_NAME, CONFIG_DB_PATH, NULL, NULL);
    if (err != RT_EOK) {
        LOG_I("file mode kvdb init (%s/%s) failed!", CONFIG_DB_PATH, CONFIG_DB_NAME);
        return RT_FALSE;
    }

    LOG_I("config db %s/%s init success!", CONFIG_DB_PATH, CONFIG_DB_NAME);
    return RT_TRUE;
}

rt_bool_t kv_config_init()
{
    if (inited) {
        return inited;
    }

    rt_mutex_init(&config_mtx, "cfg_mtx", RT_IPC_FLAG_FIFO);

    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    // rt_mutex_take(&config_mtx, RT_WAITING_FOREVER);
    inited = kv_config_init_locked();
    // rt_mutex_release(&config_mtx);
    return inited;
}

rt_bool_t kv_config_set(const char* key, const char* value)
{
    rt_err_t err = RT_FALSE;
    if (key == RT_NULL || value == RT_NULL) {
        LOG_E("%s: invalid params!", __func__);
        return RT_FALSE;
    }

    struct fdb_blob blob = {0};
    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    rt_mutex_take(&config_mtx, RT_WAITING_FOREVER);

    if (inited == RT_FALSE) {
        inited = kv_config_init_locked();
    }

    err = fdb_kv_set_blob(&config_db, key, fdb_blob_make(&blob, value, rt_strlen(value) + 1));
    rt_mutex_release(&config_mtx);
    return err == FDB_NO_ERR;
}

rt_bool_t kv_config_get(const char* key, char* value, rt_size_t val_len)
{
    rt_size_t get_len = 0;

    struct fdb_blob blob = {0};
    // LOG_I("%s:%d take mutex ...", __func__, __LINE__);
    rt_mutex_take(&config_mtx, RT_WAITING_FOREVER);

    if (inited == RT_FALSE) {
        inited = kv_config_init_locked();
    }

    get_len = fdb_kv_get_blob(&config_db, key, fdb_blob_make(&blob, value, val_len));
    rt_mutex_release(&config_mtx);
    return get_len > 0;
}

int kv_config_test(int argc, char* argv[])
{
    if (argc <= 2) {
        rt_kprintf("Usage: %s <get|set> <key> [value]\n", argv[0]);
        return -1;
    }

    LOG_I("kv_config init ...");
    kv_config_init();

    char* func = argv[1];
    char* key = argv[2];
    if (rt_strcmp(func, "get") == 0) {
        char* value = rt_calloc(128, sizeof(char));
        kv_config_get(key, value, 128);
        rt_kprintf("got %s -> %s\n", key, value);
        rt_free(value);
        return 0;
    }

    if (rt_strcmp(func, "set") == 0) {
        if (argc <= 3) {
            rt_kprintf("Usage: %s set key value\n", argv[0]);
        }
        char* value = argv[3];
        kv_config_set(key, value);
        rt_kprintf("set %s -> %s\n", key, value);
        return 0;
    }
    return 0;
}
MSH_CMD_EXPORT(kv_config_test, kv config test);