/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* JC电机CAN协议 */
#define JC_CMD_ID(motor_id)   (0x600U + (motor_id))
#define JC_RESP_ID(motor_id)  (0x580U + (motor_id))

/* 命令字 */
#define JC_CMD_WRITE_4B  0x23U
#define JC_CMD_WRITE_2B  0x2BU
#define JC_CMD_READ_4B   0x43U
#define JC_CMD_READ_2B   0x4BU

/* 寄存器地址 */
#define JC_REG_CTRL_MODE  0x0060U  /* 控制模式: 0=力矩, 1=速度, 2=位置 */
#define JC_REG_TORQUE     0x0020U  /* 力矩设定 ×100 Nm */
#define JC_REG_VELOCITY   0x0021U  /* 速度设定 ×100 rpm */
#define JC_REG_POSITION   0x0023U  /* 位置设定 ×100 deg */
#define JC_REG_ENABLE     0x00A2U  /* 进入闭环 */
#define JC_REG_DISABLE    0x00A0U  /* 空闲 */

/* 反馈数据结构 */
typedef struct {
    float position;   /* 度 */
    float velocity;   /* rpm */
    float current;    /* A */
    float voltage;    /* V */
    float driver_temperature;/* ℃ */
    float motor_temperature; /* ℃ */
    uint8_t updated;
} jc_feedback_t;

extern jc_feedback_t g_motor_fb[7]; /* index 1-6 对应电机ID */

void bsp_can_init(void);
HAL_StatusTypeDef bsp_can_send_checked(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data);
void bsp_can_send(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data);
void bsp_can_send_pos(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data);
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif
