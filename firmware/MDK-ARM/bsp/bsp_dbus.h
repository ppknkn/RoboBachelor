/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __BSP_DBUS_H__
#define __BSP_DBUS_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* DBUS帧长度 */
#define DBUS_FRAME_LEN  18

/* 通道原始值范围: 364~1684, 中值1024 */
typedef struct {
    int16_t ch[4];    /* 通道0-3: 左右摇杆, 范围-660~660 */
    uint8_t sw1;      /* 左拨杆: 1=上, 2=中, 3=下 */
    uint8_t sw2;      /* 右拨杆: 1=上, 2=中, 3=下 */
    uint8_t online;   /* 遥控器在线标志 */
} dbus_data_t;

extern dbus_data_t g_rc;

/* 需在CubeMX中配置USART1: 100000bps, 8E2, RX only, DMA循环模式 */
void bsp_dbus_init(void);

void dbus_uart_rx_callback(uint16_t Size);
void dbus_uart_error_callback(void);
#endif
