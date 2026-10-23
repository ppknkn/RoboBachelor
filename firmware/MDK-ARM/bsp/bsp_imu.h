/* SPDX-License-Identifier: GPL-3.0-only */
/* New IMU backend; existing consumer axes and units retained. */
#ifndef ROBO_IMU_H
#define ROBO_IMU_H
#include <stdint.h>
typedef struct {
    int16_t ax, ay, az; /* raw +/-8g accelerometer counts */
    int16_t mx, my, mz; /* raw IST8310 counts */
    float temp;        /* Celsius */
    float wx, wy, wz;  /* rad/s: legacy scale raw/16.384/57.3 */
    float vx, vy, vz;  /* reserved */
    float rol, pit, yaw; /* degrees, existing sign convention */
} imu_t;
extern imu_t imu;
uint8_t mpu_device_init(void);
void mpu_get_data(void);
void mpu_offset_call(void);
void init_quaternion(void);
void imu_ahrs_update(void);
void imu_attitude_update(void);
uint8_t imu_is_healthy(void);
#endif
