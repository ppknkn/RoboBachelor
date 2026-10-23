/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __JC_MOTOR_H__
#define __JC_MOTOR_H__

#include "bsp_can.h"

typedef enum {
    MODE_TORQUE   = 0,
    MODE_VELOCITY = 1,
    MODE_POSITION = 3,
} jc_ctrl_mode_t;

/* 电机ID定义 */
/* 右腿: M1=后大腿, M2=前大腿 */
/* 左腿: M3=前大腿, M4=后大腿 */
#define MOTOR_R_BACK   1
#define MOTOR_R_FRONT  2
#define MOTOR_L_FRONT  3
#define MOTOR_L_BACK   4
#define MOTOR_L_WHEEL  5
#define MOTOR_R_WHEEL  6


/* Vendor action registers require value 1; enqueue is not an ACK. */
HAL_StatusTypeDef jc_motor_set_enabled_checked(uint8_t id, uint8_t enabled);
void jc_motor_enable(uint8_t id);
void jc_motor_disable(uint8_t id);
void jc_motor_set_mode(uint8_t id, jc_ctrl_mode_t mode);
void jc_motor_set_torque(uint8_t id, float torque_nm);
void jc_motor_set_velocity(uint8_t id, float vel_rpm);
void jc_motor_set_position(uint8_t id, float pos_deg);
void jc_motor_init_all(void);
void jc_motor_request_feedback(uint8_t motor_id);
void jc_motor_request_all_feedback(void);
#endif
