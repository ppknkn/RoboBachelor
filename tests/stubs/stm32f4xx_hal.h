/* SPDX-License-Identifier: GPL-3.0-only */
/* Host tests only. No device access or STM32 implementation. */
#ifndef TEST_STM32_HAL_H
#define TEST_STM32_HAL_H
#include <stdint.h>
typedef enum { HAL_OK=0, HAL_ERROR=1, HAL_BUSY=2, HAL_TIMEOUT=3 } HAL_StatusTypeDef;
typedef struct { int unused; } CAN_HandleTypeDef;
typedef struct { int unused; } SPI_HandleTypeDef;
#define GPIOF ((void *)1)
#define GPIO_PIN_6 64U
#define GPIO_PIN_RESET 0U
#define GPIO_PIN_SET 1U
void HAL_GPIO_WritePin(void *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *, uint8_t *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *, uint8_t *, uint16_t, uint32_t);
uint32_t HAL_GetTick(void);
typedef struct { uint32_t StdId, ExtId, IDE, RTR, DLC, TransmitGlobalTime; } CAN_TxHeaderTypeDef;
typedef struct {
    uint32_t FilterBank, FilterMode, FilterScale, FilterIdHigh, FilterIdLow;
    uint32_t FilterMaskIdHigh, FilterMaskIdLow, FilterFIFOAssignment, FilterActivation;
} CAN_FilterTypeDef;
#define CAN_ID_STD 0U
#define CAN_RTR_DATA 0U
#define CAN_FILTERMODE_IDMASK 0U
#define CAN_FILTERSCALE_32BIT 0U
#define CAN_RX_FIFO0 0U
#define CAN_IT_RX_FIFO0_MSG_PENDING 1U
#define ENABLE 1U
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *, CAN_TxHeaderTypeDef *, uint8_t *, uint32_t *);
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *, CAN_FilterTypeDef *);
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *);
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *, uint32_t);
void HAL_Delay(uint32_t);
#endif
