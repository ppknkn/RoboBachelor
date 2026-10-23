/* SPDX-License-Identifier: GPL-3.0-only */
#include "bsp_tune.h"
#include "balance_ctrl.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "state_estimator.h"

extern UART_HandleTypeDef huart8;

#define TUNE_RX_LEN 64
static uint8_t tune_rx_buf[TUNE_RX_LEN];

// 用于向串口助手回复确认信息的缓冲区
static char tx_msg[64];
static char pending_cmd[TUNE_RX_LEN];
static volatile uint8_t command_pending;
static uint8_t tuning_allowed;

void bsp_tune_init(void)
{
    // 开启 UART8 的空闲中断 DMA 接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart8, tune_rx_buf, TUNE_RX_LEN);
}

// 指令解析函数
static void parse_tune_cmd(char *cmd)
{
    char *end;
    float val;
    if (!tuning_allowed || g_state.robot_state != ROBOT_ESTOP) {
        snprintf(tx_msg, sizeof(tx_msg), "Rejected: select STOP before tuning\r\n");
        goto reply;
    }
    if (strlen(cmd) < 5) {
        snprintf(tx_msg, sizeof(tx_msg), "Rejected: malformed command\r\n");
        goto reply;
    }
    val = strtof(cmd + 4, &end);
    if (end == cmd + 4 || *end != '\0' || !isfinite(val)) {
        snprintf(tx_msg, sizeof(tx_msg), "Rejected: finite number required\r\n");
        goto reply;
    }
    
    // 解析直立环 P: 发送 "BKP=1250"
    if (strncmp(cmd, "BKP=", 4) == 0) {
 
        balance_kp = val;
        snprintf(tx_msg, sizeof(tx_msg), "OK! balance_kp = %.2f\r\n", balance_kp);
    }
    // 解析直立环 D: 发送 "BKD=65"
    else if (strncmp(cmd, "BKD=", 4) == 0) {

        balance_kd = val;
        snprintf(tx_msg, sizeof(tx_msg), "OK! balance_kd = %.2f\r\n", balance_kd);
    }
    // 解析速度环 P: 发送 "VKP=12.5"
    else if (strncmp(cmd, "VKP=", 4) == 0) {

        vel_kp = val;
        snprintf(tx_msg, sizeof(tx_msg), "OK! vel_kp = %.2f\r\n", vel_kp);
    }
    // 速度环已改为 P-only 串联结构，"VKI=" 命令已废弃
    // 当前不支持 PKP= / PKI=；位置 PI 默认值在 state_estimator.c 中。
    // 解析转向环 P: 发送 "TKP=0.2"
    else if (strncmp(cmd, "TKP=", 4) == 0) {

        turn_kp = val;
        snprintf(tx_msg, sizeof(tx_msg), "OK! turn_kp = %.2f\r\n", turn_kp);
    }
    // 解析零点偏置: 发送 "TOF=0.1"
    else if (strncmp(cmd, "TOF=", 4) == 0) {

        theta_offset = val;
        snprintf(tx_msg, sizeof(tx_msg), "OK! theta_offset = %.2f\r\n", theta_offset);
    }

    else {
        snprintf(tx_msg, sizeof(tx_msg), "Unknown Command!\r\n");
    }

reply:
    // Reply only from main context; never block inside the UART IRQ.
    HAL_UART_Transmit(&huart8, (uint8_t*)tx_msg, strlen(tx_msg), 5);
}


/* UART IRQ only queues a bounded tuning command. */
void tune_uart_rx_callback(uint16_t Size)
{
    if (Size > 0 && Size < TUNE_RX_LEN) {
        tune_rx_buf[Size] = '\0';
        while (Size > 0 && (tune_rx_buf[Size-1] == '\r' || tune_rx_buf[Size-1] == '\n'))
            tune_rx_buf[--Size] = '\0';
        if (!command_pending) {
            memcpy(pending_cmd, tune_rx_buf, Size + 1U);
            command_pending = 1;
        }
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart8, tune_rx_buf, TUNE_RX_LEN);
}

void bsp_tune_process(uint8_t allow_tuning)
{
    if (command_pending) {
        tuning_allowed = allow_tuning;
        parse_tune_cmd(pending_cmd);
        command_pending = 0;
    }
}

void tune_uart_error_callback(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart8, tune_rx_buf, TUNE_RX_LEN);
}
