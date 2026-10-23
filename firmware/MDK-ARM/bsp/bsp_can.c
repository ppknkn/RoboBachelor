/* SPDX-License-Identifier: GPL-3.0-only */
#include "bsp_can.h"
#include "can.h"

jc_feedback_t g_motor_fb[7] = {0};

static CAN_TxHeaderTypeDef tx_header = {
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = 8,
};

void bsp_can_init(void)
{
    CAN_FilterTypeDef filter = {
        .FilterBank           = 0,
        .FilterMode           = CAN_FILTERMODE_IDMASK,
        .FilterScale          = CAN_FILTERSCALE_32BIT,
        .FilterIdHigh         = 0x0000,
        .FilterIdLow          = 0x0000,
        .FilterMaskIdHigh     = 0x0000,
        .FilterMaskIdLow      = 0x0000,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterActivation     = ENABLE,
    };
    HAL_CAN_ConfigFilter(&hcan1, &filter);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

HAL_StatusTypeDef bsp_can_send_checked(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data)
{
    uint8_t tx_data[8];
    uint32_t mailbox;

    tx_header.StdId = JC_CMD_ID(motor_id);

    tx_data[0] = cmd;
    tx_data[1] = (reg >> 8) & 0xFF;  /* 寄存器高字节 */
    tx_data[2] = reg & 0xFF;          /* 寄存器低字节 */
    tx_data[3] = 0x00;
    tx_data[4] = (data >> 8) & 0xFF;  /* 数据高字节 */
    tx_data[5] = data & 0xFF;          /* 数据低字节 */
    tx_data[6] = 0x00;
    tx_data[7] = 0x00;

    return HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &mailbox);
}

void bsp_can_send(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data)
{
    (void)bsp_can_send_checked(motor_id, cmd, reg, data);
}

void bsp_can_send_pos(uint8_t motor_id, uint8_t cmd, uint16_t reg, int32_t data)
{
    uint8_t tx_data[8];
    uint32_t mailbox;

    tx_header.StdId = JC_CMD_ID(motor_id);

    tx_data[0] = cmd;
    tx_data[1] = (reg >> 8) & 0xFF;  /* 寄存器高字节 */
    tx_data[2] = reg & 0xFF;          /* 寄存器低字节 */
    tx_data[3] = 0x00;
    tx_data[4] = (data >> 24) & 0xFF; /* 数据最高字节 */
    tx_data[5] = (data >> 16) & 0xFF;
    tx_data[6] = (data >> 8) & 0xFF;
    tx_data[7] = data & 0xFF;          /* 数据最低字节 */

    HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &mailbox);
}
/* Active CAN RX routing is in Src/stm32f4xx_it.c. */
