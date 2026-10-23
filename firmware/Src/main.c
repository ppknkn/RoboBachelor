/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_imu.h"
#include "bsp_dbus.h"
#include "bsp_tune.h"
#include "dm4310.h"
#include "jc_motor.h"
#include "state_estimator.h"
#include "balance_ctrl.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart8;
static uint32_t last_ctrl_tick = 0;
static uint32_t last_print_tick = 0;
static char dbg_buf[256];
static uint8_t motors_enabled = 1;
float target_v       = 0.0f; // 历史全局量；实际控制使用 g_state.target_v (m/s)
float target_turn    = 0.0f; // 目标转向角速度 (rad/s)
float target_leg_len = 100.0f; // 目标腿长 (mm)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI5_Init();
  MX_USART6_UART_Init();
  MX_UART8_Init();
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
  bsp_dbus_init();
  bsp_tune_init();
  if (mpu_device_init() != 0) Error_Handler(); /* No motor enable after sensor init failure. */
  mpu_get_data();        /* 先读一次传感器，填充 imu 数据供 init_quaternion() 使用 */
  init_quaternion();
  /* 等待IMU收敛（2秒） */
  for (int i = 0; i < 1000; i++) {
      mpu_get_data();
      imu_ahrs_update();
      imu_attitude_update();
      HAL_Delay(2);
  }
  bsp_can_init();  /* 统一CAN初始化，达妙和JC共用 */
  HAL_Delay(100);
  /* 使能关节电机 */
  for (uint8_t id = 1; id <= 4; id++) {
      dm4310_enable(id);  
      HAL_Delay(5);
  }
  /* 使能轮毂电机（力矩模式） */
  jc_motor_set_mode(MOTOR_L_WHEEL, MODE_TORQUE);
  HAL_Delay(5);
  jc_motor_enable(MOTOR_L_WHEEL);
  HAL_Delay(5);
  jc_motor_set_mode(MOTOR_R_WHEEL, MODE_TORQUE);
  HAL_Delay(5);
  jc_motor_enable(MOTOR_R_WHEEL);
  HAL_Delay(5);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    mpu_get_data();
    imu_ahrs_update();
    imu_attitude_update();
    robot_logic_update();
    bsp_tune_process(g_state.robot_state == ROBOT_ESTOP);
    /* 2ms控制节拍 */
    if (HAL_GetTick() - last_ctrl_tick >= 2)
    {
      static uint8_t joint_poll_id = 1;
      static uint8_t wheel_poll_id = 5;
      static uint32_t last_est_tick = 0;
      uint32_t now = HAL_GetTick();
      float dt = (float)(now - last_est_tick) / 1000.0f;
      if (dt < 0.001f) dt = 0.001f;
      if (dt > 0.005f) dt = 0.005f;
      last_est_tick = now;

      /* 轮毂电机速度反馈（rpm -> m/s） */
      const float RPM_TO_MS = (3.14159265f * 0.066f) / 60.0f;
      g_state.v_wheel_L = g_motor_fb[MOTOR_L_WHEEL].velocity * RPM_TO_MS;
      g_state.v_wheel_R = -g_motor_fb[MOTOR_R_WHEEL].velocity * RPM_TO_MS;

      state_estimator_update(dt);

      /* 跌倒检测：超过1rad则停机 */
////////////////////////////////////////////////////////////////////
      if (motors_enabled && (!imu_is_healthy() || !isfinite(g_state.theta) || fabsf(g_state.theta) > 1.0f)) {
///////////////////////////////////////////////////////////////////
        for (uint8_t id = 1; id <= 4; id++){
            dm4310_disable(id);
            HAL_Delay(1);
        } 
        jc_motor_disable(MOTOR_L_WHEEL);
        HAL_Delay(1);
        jc_motor_disable(MOTOR_R_WHEEL);
        motors_enabled = 0;
      }
      /* Preserve original STOP toggle; suppress new position/velocity targets.
       * STOP does not introduce a new motor enable or re-arm workflow. */
      if (!motors_enabled || g_state.robot_state == ROBOT_ESTOP) {
        jc_motor_set_torque(MOTOR_L_WHEEL, 0);
        jc_motor_set_torque(MOTOR_R_WHEEL, 0);
      }
      if (motors_enabled && g_state.robot_state != ROBOT_ESTOP) {
        float tau_fL, tau_bL, tau_fR, tau_bR;
        float pos_fL, pos_bL, pos_fR, pos_bR;
        float vel_fL, vel_bL, vel_fR, vel_bR;
        float tau_wL, tau_wR;
        balance_ctrl_update(
            & (g_state.robot_state),
            g_state.target_v, g_state.target_turn, g_state.target_leg_len,
            g_state.l_L, g_state.phi_L,      /* 直接传 mm, leg_ik 期望 mm */
            g_state.l_R, g_state.phi_R,
            g_state.theta, g_state.dtheta,
            g_state.dyaw,
            g_state.v_wheel_L, g_state.v_wheel_R,
            &tau_fL, &tau_bL, &tau_fR, &tau_bR,
            &pos_fL, &pos_bL, &pos_fR, &pos_bR,
            &vel_fL, &vel_bL, &vel_fR, &vel_bR,
            &tau_wL, &tau_wR);

        /* 起身时 kp=0 纯速度控制, 平衡时位置跟踪 */
        float pose_kp = leg_pos_kp;
        float pose_kd = leg_pos_kd;
        if (g_state.robot_state == ROBOT_STANDUP) {
            pose_kp = 0.0f;
            pose_kd = 2.0f;
        }

        switch (joint_poll_id) {
          case 1: dm4310_mit_send(1, pos_fL, vel_fL, pose_kp, pose_kd,  tau_fL); break;
          case 2: dm4310_mit_send(2, pos_bL, vel_bL, pose_kp, pose_kd, -tau_bL); break;
          case 3: dm4310_mit_send(3, pos_bR, vel_bR, pose_kp, pose_kd, -tau_bR); break;
          case 4: dm4310_mit_send(4, pos_fR, vel_fR, pose_kp, pose_kd,  tau_fR); break;
        }
        switch (wheel_poll_id) {
          case 5: jc_motor_set_torque(MOTOR_L_WHEEL, tau_wL); break;
          case 6: jc_motor_set_torque(MOTOR_R_WHEEL, tau_wR); break;
        }
      }

      if (++joint_poll_id > 4) joint_poll_id = 1;
      if (++wheel_poll_id > 6) wheel_poll_id = 5;
      last_ctrl_tick = HAL_GetTick();
    }
    if (HAL_GetTick() - last_print_tick >= 300U) {
      int len = snprintf(dbg_buf, sizeof(dbg_buf),
          "st:%d|th:%.2f|dth:%.2f|yaw:%.2f|dyaw:%.2f\r\n",
          g_state.robot_state, g_state.theta, g_state.dtheta, g_state.yaw, g_state.dyaw);
      last_print_tick = HAL_GetTick();
      if (len > 0 && (size_t)len < sizeof(dbg_buf))
          HAL_UART_Transmit(&huart8, (uint8_t *)dbg_buf, (uint16_t)len, 5U);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) 
    {
        dbus_uart_rx_callback(Size);
    }
    else if (huart->Instance == UART8) 
    {
        tune_uart_rx_callback(Size);
    }
    // 以后如果加了视觉串口 UART6，直接在这里加一个 else if (huart->Instance == USART6) vision_uart_rx_callback(Size); 即可
}
/**
  * @brief  全局串口错误中断回调 (总路由器)
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) 
    {
        dbus_uart_error_callback();
    }
    else if (huart->Instance == UART8) 
    {
        tune_uart_error_callback();
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
