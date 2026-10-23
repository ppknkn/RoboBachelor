/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __BSP_TUNE_H__
#define __BSP_TUNE_H__
#include <stdint.h>

void bsp_tune_process(uint8_t allow_tuning);
void bsp_tune_init(void);

void tune_uart_rx_callback(uint16_t Size);
void tune_uart_error_callback(void); // 新增
#endif
