/* SPDX-License-Identifier: GPL-3.0-only */
#include "jc_motor.h"
#include "bsp_can.h"
#include "stm32f4xx_hal.h"

/* 零点偏移 (度), 标定后填入实测值 */
float motor_offset[7] = {0, -836.8, 706.8, -105.2, -385.0, 0, 0};

HAL_StatusTypeDef jc_motor_set_enabled_checked(uint8_t id, uint8_t enabled)
{
    if (id < 1 || id > 6) return HAL_ERROR;
    return bsp_can_send_checked(id, JC_CMD_WRITE_2B,
                                enabled ? JC_REG_ENABLE : JC_REG_DISABLE, 1);
}

void jc_motor_enable(uint8_t id)
{
    (void)jc_motor_set_enabled_checked(id, 1);
	HAL_Delay(2);
}

void jc_motor_disable(uint8_t id)
{
    (void)jc_motor_set_enabled_checked(id, 0);
	HAL_Delay(2);
}

void jc_motor_set_mode(uint8_t id, jc_ctrl_mode_t mode)
{
    bsp_can_send(id, JC_CMD_WRITE_2B, JC_REG_CTRL_MODE, (int32_t)mode);
	HAL_Delay(2);
}

void jc_motor_set_torque(uint8_t id, float torque_nm)
{
    bsp_can_send(id, JC_CMD_WRITE_2B, JC_REG_TORQUE, (int32_t)(torque_nm * 100.0f));
}

void jc_motor_set_velocity(uint8_t id, float vel_rpm)
{
    bsp_can_send(id, JC_CMD_WRITE_4B, JC_REG_VELOCITY, (int32_t)(vel_rpm * 100.0f));
	HAL_Delay(2);
}

void jc_motor_init_all(void)
{
    bsp_can_init();
    HAL_Delay(10);

    for (uint8_t id = MOTOR_R_BACK; id <= MOTOR_L_BACK; id++) {
        jc_motor_set_mode(id, MODE_POSITION);
        jc_motor_enable(id);
    }

    for (uint8_t id = MOTOR_L_WHEEL; id <= MOTOR_R_WHEEL; id++) {
        jc_motor_set_mode(id, MODE_TORQUE);
        jc_motor_enable(id);
    }
}
/* 主动查询电机位置、速度、电流 */
void jc_motor_request_feedback(uint8_t motor_id)
{
    bsp_can_send(motor_id, JC_CMD_READ_4B, 0x0008, 0); /* 位置 */
	HAL_Delay(2);
    bsp_can_send(motor_id, JC_CMD_READ_4B, 0x0006, 0); /* 速度 */
	HAL_Delay(2);
    bsp_can_send(motor_id, JC_CMD_READ_2B, 0x0005, 0); /* 电流 */
	HAL_Delay(2);
    bsp_can_send(motor_id, JC_CMD_READ_2B, 0x0004, 0); /* 电压 */
	HAL_Delay(2);
}
void jc_motor_set_position(uint8_t id, float pos_deg)
{
    bsp_can_send_pos(id, JC_CMD_WRITE_4B, JC_REG_POSITION, (int32_t)(pos_deg * 100.0f));
    HAL_Delay(2);
}


void jc_motor_request_all_feedback(void)
{
    for (uint8_t id = MOTOR_R_BACK; id <= MOTOR_L_BACK; id++) {
        jc_motor_request_feedback(id);
    }
    for (uint8_t id = MOTOR_L_WHEEL; id <= MOTOR_R_WHEEL; id++) {
        jc_motor_request_feedback(id);
    }
}
