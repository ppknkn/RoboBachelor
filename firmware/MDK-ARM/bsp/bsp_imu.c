/* SPDX-License-Identifier: GPL-3.0-only */
/* New RoboBachelor implementation using MIT LibDriver and genuine GPL Mahony.
 * No A-board example implementation included. See docs/imu-backend.md. */
#include "bsp_imu.h"
#include "imu_port.h"
#include "imu_ist8310_aux.h"
#include "driver_mpu6500_registers.h"
#include "MahonyAHRS.h"
#include <math.h>
#include <string.h>

imu_t imu;
static mpu6500_handle_t sensor;
static int32_t bias[3];
static float quat[4] = {1, 0, 0, 0};
static uint32_t sample_ms, magnetic_ms, fusion_ms;
static uint8_t configured, calibrated, sample_ok, magnetic_ok, orientation_ok, fusion_ok, magnetic_new;

static int16_t be16(const uint8_t *p)
{
    uint16_t v = (uint16_t)((uint16_t)p[0] * 256U + p[1]);
    return (int16_t)(v < 32768U ? (int32_t)v : (int32_t)v - 65536);
}
static uint8_t put(uint8_t reg, uint8_t value)
{ return mpu6500_set_reg(&sensor, reg, &value, 1); }

void mpu_offset_call(void)
{
    uint8_t data[14];
    int32_t sum[3] = {0, 0, 0};
    calibrated = 0;
    if (!configured) return;
    for (unsigned n = 0; n < 300; ++n) {
        if (mpu6500_get_reg(&sensor, MPU6500_REG_ACCEL_XOUT_H, data, 14)) return;
        for (unsigned a = 0; a < 3; ++a) sum[a] += be16(data + 8 + a*2);
        imu_port_delay(5);
    }
    for (unsigned a = 0; a < 3; ++a) bias[a] = sum[a] / 300;
    calibrated = 1;
}
uint8_t mpu_device_init(void)
{
    /* Explicit project settings, not LibDriver example defaults. */
    static const uint8_t config[][2] = {
        {MPU6500_REG_PWR_MGMT_1, 3}, {MPU6500_REG_PWR_MGMT_2, 0},
        {MPU6500_REG_CONFIG, 4}, {MPU6500_REG_GYRO_CONFIG, 0x18},
        {MPU6500_REG_ACCEL_CONFIG, 0x10}, {MPU6500_REG_ACCEL_CONFIG2, 2},
        {MPU6500_REG_USER_CTRL, 0x30}
    };
    memset(&imu, 0, sizeof imu);
    configured = calibrated = sample_ok = magnetic_ok = orientation_ok = fusion_ok = 0;
    memset(bias, 0, sizeof bias);
    imu_port_bind(&sensor);
    if (mpu6500_init(&sensor)) return 1;
    for (unsigned n = 0; n < sizeof config / sizeof config[0]; ++n) {
        uint8_t actual;
        if (put(config[n][0], config[n][1])) return 2;
        imu_port_delay(1);
        if (mpu6500_get_reg(&sensor, config[n][0], &actual, 1) || actual != config[n][1]) return 3;
    }
    if (imu_ist8310_configure(&sensor)) return 4;
    configured = 1;
    mpu_offset_call();
    if (!calibrated) return 5;
    /* AUX and the host loop are asynchronous. Wait for the first usable
     * magnetic sample instead of depending on one arbitrary DRDY phase. */
    for (unsigned n = 0; n < 50; ++n) {
        mpu_get_data();
        if (sample_ok && magnetic_ok) return 0;
        imu_port_delay(2);
    }
    return 6;
}
void mpu_get_data(void)
{
    uint8_t data[14], fresh;
    int16_t mag[3];
    sample_ok = magnetic_new = 0;
    if (!configured || !calibrated) return;
    if (mpu6500_get_reg(&sensor, MPU6500_REG_ACCEL_XOUT_H, data, 14) ||
        imu_ist8310_read(&sensor, mag, &fresh)) return;
    imu.ax = be16(data); imu.ay = be16(data+2); imu.az = be16(data+4);
    imu.temp = 21.0f + be16(data+6) / 333.87f;
    imu.wx = ((int32_t)be16(data+8) - bias[0]) / 16.384f / 57.3f;
    imu.wy = ((int32_t)be16(data+10) - bias[1]) / 16.384f / 57.3f;
    imu.wz = ((int32_t)be16(data+12) - bias[2]) / 16.384f / 57.3f;
    if (fresh) {
        imu.mx = mag[0]; imu.my = mag[1]; imu.mz = mag[2];
        magnetic_ok = (mag[0] || mag[1] || mag[2]);
        magnetic_ms = imu_port_now();
        magnetic_new = magnetic_ok;
    }
    sample_ms = imu_port_now();
    sample_ok = (imu.ax || imu.ay || imu.az);
}
void init_quaternion(void)
{
    /* Standard tilt compensation and ZYX half-angle construction. */
    float norm = sqrtf((float)imu.ax*imu.ax + (float)imu.ay*imu.ay + (float)imu.az*imu.az);
    float r, p, y, north, east, cr, sr, cp, sp, cy, sy;
    orientation_ok = 0;
    if (!sample_ok || !magnetic_ok || norm < 100) return;
    r = atan2f((float)imu.ay, (float)imu.az);
    p = asinf(fmaxf(-1, fminf(1, -(float)imu.ax / norm)));
    north = imu.mx*cosf(p) + imu.my*sinf(r)*sinf(p) + imu.mz*cosf(r)*sinf(p);
    east = imu.my*cosf(r) - imu.mz*sinf(r);
    y = atan2f(-east, north);
    cr = cosf(r/2); sr = sinf(r/2); cp = cosf(p/2); sp = sinf(p/2); cy = cosf(y/2); sy = sinf(y/2);
    quat[0] = cr*cp*cy + sr*sp*sy; quat[1] = sr*cp*cy - cr*sp*sy;
    quat[2] = cr*sp*cy + sr*cp*sy; quat[3] = cr*cp*sy - sr*sp*cy;
    MahonyReset(); fusion_ms = imu_port_now(); orientation_ok = fusion_ok = 1;
}
void imu_ahrs_update(void)
{
    uint32_t elapsed = (uint32_t)(imu_port_now() - fusion_ms);
    fusion_ms = imu_port_now(); fusion_ok = 0;
    if (!sample_ok || !orientation_ok || elapsed > 100U) return;
    if (elapsed) {
        MahonySetPeriod(elapsed * 0.001f);
        MahonyAHRSupdate(quat, imu.wx, imu.wy, imu.wz, imu.ax, imu.ay, imu.az,
                        magnetic_new ? imu.mx : 0, magnetic_new ? imu.my : 0, magnetic_new ? imu.mz : 0);
        magnetic_new = 0;
    }
    fusion_ok = 1;
    for (unsigned a = 0; a < 4; ++a) if (!isfinite(quat[a])) orientation_ok = fusion_ok = 0;
}
void imu_attitude_update(void)
{
    float w=quat[0], x=quat[1], y=quat[2], z=quat[3];
    imu.rol = atan2f(2*(w*x+y*z), 1-2*(x*x+y*y)) * 57.3f;
    imu.pit = -asinf(fmaxf(-1, fminf(1, 2*(w*y-x*z)))) * 57.3f;
    imu.yaw = -atan2f(2*(w*z+x*y), 1-2*(y*y+z*z)) * 57.3f;
}
uint8_t imu_is_healthy(void)
{
    uint32_t now = imu_port_now();
    return configured && calibrated && sample_ok && magnetic_ok && orientation_ok && fusion_ok &&
           (uint32_t)(now-sample_ms) <= 20U && (uint32_t)(now-magnetic_ms) <= 200U;
}
