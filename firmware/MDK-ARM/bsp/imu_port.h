/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ROBO_IMU_PORT_H
#define ROBO_IMU_PORT_H
#include "driver_mpu6500.h"
void imu_port_bind(mpu6500_handle_t *handle);
uint32_t imu_port_now(void);
void imu_port_delay(uint32_t ms);
#endif
