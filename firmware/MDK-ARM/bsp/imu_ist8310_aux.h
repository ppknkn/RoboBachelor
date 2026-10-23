/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ROBO_IMU_IST_AUX_H
#define ROBO_IMU_IST_AUX_H
#include "driver_mpu6500.h"
uint8_t imu_ist8310_configure(mpu6500_handle_t *sensor);
uint8_t imu_ist8310_read(mpu6500_handle_t *sensor,int16_t out[3],uint8_t *fresh);
#endif
