/**
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author       Notes
 * 2020-12-12      Wayne        First version
 *
 ******************************************************************************/

#include <rtconfig.h>
#include <rtdevice.h>

#include "ui_thread_init.h"

#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#if defined(RT_USING_PIN)
#include <drv_gpio.h>

/* defined the INDICATOR_LED pin: PH2 */
#define INDICATOR_LED NU_GET_PININDEX(NU_PH, 2)

#define MATRIX_COL_NUM 2
#define MATRIX_ROW_NUM 3

/* defined the KEY_COl_0 pin: PB4 */
#define KEY_COL_0 NU_GET_PININDEX(NU_PB, 4)

/* defined the KEY_COl_1 pin: PB5 */
#define KEY_COL_1 NU_GET_PININDEX(NU_PB, 5)

/* defined the KEY_ROW_0 pin: PF10 */
#define KEY_ROW_0 NU_GET_PININDEX(NU_PF, 10)

/* defined the KEY_ROW_1 pin: PE15 */
#define KEY_ROW_1 NU_GET_PININDEX(NU_PE, 15)

/* defined the KEY_ROW_2 pin: PE14 */
#define KEY_ROW_2 NU_GET_PININDEX(NU_PE, 14)

uint32_t au32KeyMatrix_Col[MATRIX_COL_NUM] = {
    KEY_COL_0,
    KEY_COL_1,
};

uint32_t au32KeyMatrix_Row[MATRIX_ROW_NUM] = {
    KEY_ROW_0,
    KEY_ROW_1,
    KEY_ROW_2,
};

const char* szKeyLabel[] = {
    "K1", "K2", "K3", "K4", "K5", "K6",
};

const uint8_t key_code[] = {
    KEY_K1, KEY_K2, KEY_K3, KEY_K4, KEY_K5, KEY_K6,
};

static void nu_key_matrix_cb(void* args)
{
    uint32_t ri = (uint32_t)args;
    int ci;
    for (ci = 0; ci < MATRIX_COL_NUM; ci++) {
        /* Find column bit is low. */
        if (!rt_pin_read(au32KeyMatrix_Col[ci])) {
            break;
        }
    }
    rt_size_t index = (ci) + MATRIX_COL_NUM * ri;
    rt_kprintf("[%d %d] Pressed %s button.\n", ci, ri, szKeyLabel[index]);

    rt_mq_t key_mq = ui_get_key_mq();
    if (key_mq) {
        ui_key_msg_t msg;
        msg.key = key_code[index];
        msg.state = K_PRESSED;
        LOG_I("send key msg %d %d.", msg.key, msg.state);
        rt_mq_send(key_mq, &msg, sizeof(msg));
    }
}

static void nu_key_matrix_switch(uint32_t counter)
{
    int i;
    for (i = 0; i < MATRIX_COL_NUM; i++) {
        /* set pin value to high */
        rt_pin_write(au32KeyMatrix_Col[i], PIN_HIGH);
    }
    /* set pin value to low */
    rt_pin_write(au32KeyMatrix_Col[counter % MATRIX_COL_NUM], PIN_LOW);
}
#endif

int main(int argc, char** argv)
{
#if defined(RT_USING_PIN)
    uint32_t counter = 1;
    int i = 0;

    for (i = 0; i < MATRIX_ROW_NUM; i++) {
        /* set pin mode to input */
        rt_pin_mode(au32KeyMatrix_Row[i], PIN_MODE_INPUT_PULLUP);

        rt_pin_attach_irq(au32KeyMatrix_Row[i], PIN_IRQ_MODE_FALLING, nu_key_matrix_cb, (void*)i);
        rt_pin_irq_enable(au32KeyMatrix_Row[i], PIN_IRQ_ENABLE);
    }

    for (i = 0; i < MATRIX_COL_NUM; i++) {
        /* set  pin mode to output */
        rt_pin_mode(au32KeyMatrix_Col[i], PIN_MODE_OUTPUT);
    }

    /* set INDICATOR_LED pin mode to output */
    rt_pin_mode(INDICATOR_LED, PIN_MODE_OUTPUT);

    /* Toggle column pins in key matrix. */
    rt_base_t indicator_value = PIN_LOW;
    while (counter++ > 0) {
        rt_thread_mdelay(100);
        nu_key_matrix_switch(counter);
        if (counter % 5 == 0) {
            if ((counter / 5) & 1) {
                rt_pin_write(INDICATOR_LED, indicator_value);
                indicator_value = !indicator_value;
            }
        }
    }
#endif

    return 0;
}
