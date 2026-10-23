/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dm4310.h"
#include "bsp_can.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern DMA_HandleTypeDef hdma_uart8_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart6;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

  /* USER CODE END DMA1_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart8_rx);
  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
  * @brief This function handles CAN1 RX0 interrupts.
  */
void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX0_IRQn 0 */

  /* USER CODE END CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/**
  * @brief This function handles UART8 global interrupt.
  */
void UART8_IRQHandler(void)
{
  /* USER CODE BEGIN UART8_IRQn 0 */

  /* USER CODE END UART8_IRQn 0 */
  HAL_UART_IRQHandler(&huart8);
  /* USER CODE BEGIN UART8_IRQn 1 */

  /* USER CODE END UART8_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef hdr;
  uint8_t data[8];
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, data) != HAL_OK)
    return;
  if (hdr.IDE != CAN_ID_STD || hdr.RTR != CAN_RTR_DATA || hdr.DLC != 8U)
    return; /* Never parse truncated or unrelated frame formats. */

  /* 达妙4310反馈帧：StdId == Master ID (默认0)，靠data[0]低4位识别电机ID */
  if (hdr.StdId == 0 && (data[0] & 0x0F) >= 1 && (data[0] & 0x0F) <= 4)
  {
    dm4310_parse_rx(data);
    return;
  }

  /* JC轮毂电机反馈帧：StdId = 0x580 + motor_id */
  if ((hdr.StdId & 0xFF80U) == 0x0580U)
  {
    uint8_t id = hdr.StdId & 0x7FU;
    if (id >= 5 && id <= 6)
    {
      uint8_t id = hdr.StdId & 0x7FU;
      if (id >= 5 && id <= 6)
      {
        uint8_t cmd = data[0];
        uint16_t reg = ((uint16_t)data[1] << 8) | data[2];
        if (cmd == 0x43)
        {
          int32_t v = ((int32_t)data[4] << 24) | ((int32_t)data[5] << 16) |
                      ((int32_t)data[6] << 8) | data[7];
          if (reg == 0x0006)
            g_motor_fb[id].velocity = v / 100.0f;
          if (reg == 0x0008)
            g_motor_fb[id].position = v / 100.0f;
        }
        else if (cmd == 0x4B)
        {
          int16_t v = (int16_t)(((uint16_t)data[4] << 8) | data[5]);
          if (reg == 0x0005)
            g_motor_fb[id].current = v / 100.0f;
        }
        else if (cmd == 0x2A)
        {
          // -------------------------- 解析位置（24位有符号数，单位0.01°） --------------------------
          // 协议说明：位置范围 -83886.08° ~ 83886.07°，原始值×100
          // 数据格式：data[1] = 位置H(高8位), data[2] = 位置M(中8位), data[3] = 位置L(低8位)
          uint32_t pos_24_raw = (uint32_t)data[1] << 16 | (uint16_t)data[2] << 8 | data[3];
          int32_t pos_raw;

          // 24位有符号数 → 32位符号扩展（处理负数）
          if (pos_24_raw & 0x00800000)
          { // 符号位（第23位）为1，负数扩展高8位
            pos_raw = (int32_t)(pos_24_raw | 0xFF000000);
          }
          else
          {
            pos_raw = (int32_t)pos_24_raw;
          }

          // 单位换算：原始值 / 100 → 实际角度（°）
          g_motor_fb[id].position = pos_raw / 100.0f;

          // -------------------------- 解析速度（16位有符号数，单位rpm） --------------------------
          // 协议说明：速度范围 -10000 ~ 10000 rpm，无缩放
          // 数据格式：data[4] = 速度H(高8位), data[5] = 速度L(低8位)
          int16_t vel_raw = (int16_t)(((uint16_t)data[4] << 8) | data[5]);
          g_motor_fb[id].velocity = vel_raw;
          // 注意：如果你的速度单位需要和前面cmd=0x43保持一致（×100），请改为：
          // g_motor_fb[id].velocity = vel_raw / 100.0f;

          // -------------------------- 解析电流（16位有符号数，单位0.01A） --------------------------
          // 协议说明：电流范围 -20.00A ~ 20.00A，原始值×100
          // 数据格式：data[6] = 电流H(高8位), data[7] = 电流L(低8位)
          int16_t cur_raw = (int16_t)(((uint16_t)data[6] << 8) | data[7]);
          g_motor_fb[id].current = cur_raw / 100.0f;
        }
        g_motor_fb[id].updated = 1;
      }
    }
  }
}
/* USER CODE END 1 */
