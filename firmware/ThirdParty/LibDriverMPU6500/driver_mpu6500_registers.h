/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_mpu6500.c
 * @brief     driver mpu6500 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2024-07-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2024/07/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */


#ifndef LIBDRIVER_MPU6500_REGISTERS_H
#define LIBDRIVER_MPU6500_REGISTERS_H
#define MPU6500_REG_SELF_TEST_X_GYRO    0x00        /**< gyro self test x register */
#define MPU6500_REG_SELF_TEST_Y_GYRO    0x01        /**< gyro self test y register */
#define MPU6500_REG_SELF_TEST_Z_GYRO    0x02        /**< gyro self test z register */
#define MPU6500_REG_SELF_TEST_X_ACCEL   0x0D        /**< accel self test x register */
#define MPU6500_REG_SELF_TEST_Y_ACCEL   0x0E        /**< accel self test y register */
#define MPU6500_REG_SELF_TEST_Z_ACCEL   0x0F        /**< accel self test z register */
#define MPU6500_REG_XG_OFFSET_H         0x13        /**< gyro offset x high register */
#define MPU6500_REG_XG_OFFSET_L         0x14        /**< gyro offset x low register */
#define MPU6500_REG_YG_OFFSET_H         0x15        /**< gyro offset y high register */
#define MPU6500_REG_YG_OFFSET_L         0x16        /**< gyro offset y low register */
#define MPU6500_REG_ZG_OFFSET_H         0x17        /**< gyro offset z high register */
#define MPU6500_REG_ZG_OFFSET_L         0x18        /**< gyro offset z low register */
#define MPU6500_REG_SMPRT_DIV           0x19        /**< smprt div register */
#define MPU6500_REG_CONFIG              0x1A        /**< configure register */
#define MPU6500_REG_GYRO_CONFIG         0x1B        /**< gyro configure register */
#define MPU6500_REG_ACCEL_CONFIG        0x1C        /**< accel configure register */
#define MPU6500_REG_ACCEL_CONFIG2       0x1D        /**< accel configure 2 register */
#define MPU6500_REG_LP_ACCEL_ODR        0x1E        /**< low power accel odr register */
#define MPU6500_REG_WOM_THR             0x1F        /**< wom threshold register */
#define MPU6500_REG_MOTION_DURATION     0x20        /**< motion duration register */
#define MPU6500_REG_FIFO_EN             0x23        /**< fifo enable register */
#define MPU6500_REG_I2C_MST_CTRL        0x24        /**< i2c master ctrl register */
#define MPU6500_REG_I2C_MST_STATUS      0x36        /**< i2c master status register */
#define MPU6500_REG_I2C_MST_DELAY_CTRL  0x67        /**< i2c master delay ctrl register */
#define MPU6500_REG_I2C_SLV0_ADDR       0x25        /**< iic slave0 address register */
#define MPU6500_REG_I2C_SLV0_REG        0x26        /**< iic slave0 reg register */
#define MPU6500_REG_I2C_SLV0_CTRL       0x27        /**< iic slave0 ctrl register */
#define MPU6500_REG_I2C_SLV0_DO         0x63        /**< iic slave0 do register */
#define MPU6500_REG_I2C_SLV1_ADDR       0x28        /**< iic slave1 address register */
#define MPU6500_REG_I2C_SLV1_REG        0x29        /**< iic slave1 reg register */
#define MPU6500_REG_I2C_SLV1_CTRL       0x2A        /**< iic slave1 ctrl register */
#define MPU6500_REG_I2C_SLV1_DO         0x64        /**< iic slave1 do register */
#define MPU6500_REG_I2C_SLV2_ADDR       0x2B        /**< iic slave2 address register */
#define MPU6500_REG_I2C_SLV2_REG        0x2C        /**< iic slave2 reg register */
#define MPU6500_REG_I2C_SLV2_CTRL       0x2D        /**< iic slave2 ctrl register */
#define MPU6500_REG_I2C_SLV2_DO         0x65        /**< iic slave2 do register */
#define MPU6500_REG_I2C_SLV3_ADDR       0x2E        /**< iic slave3 address register */
#define MPU6500_REG_I2C_SLV3_REG        0x2F        /**< iic slave3 reg register */
#define MPU6500_REG_I2C_SLV3_CTRL       0x30        /**< iic slave3 ctrl register */
#define MPU6500_REG_I2C_SLV3_DO         0x66        /**< iic slave3 do register */
#define MPU6500_REG_I2C_SLV4_ADDR       0x31        /**< iic slave4 address register */
#define MPU6500_REG_I2C_SLV4_REG        0x32        /**< iic slave4 reg register */
#define MPU6500_REG_I2C_SLV4_CTRL       0x34        /**< iic slave4 ctrl register */
#define MPU6500_REG_I2C_SLV4_DO         0x33        /**< iic slave4 do register */
#define MPU6500_REG_I2C_SLV4_DI         0x35        /**< iic slave4 di register */
#define MPU6500_REG_EXT_SENS_DATA_00    0x49        /**< extern sensor data 00 register */
#define MPU6500_REG_INT_PIN_CFG         0x37        /**< interrupt pin configure register */
#define MPU6500_REG_INT_ENABLE          0x38        /**< interrupt enable register */
#define MPU6500_REG_INT_STATUS          0x3A        /**< interrupt status register */
#define MPU6500_REG_ACCEL_XOUT_H        0x3B        /**< accel xout high register */
#define MPU6500_REG_ACCEL_XOUT_L        0x3C        /**< accel xout low register */
#define MPU6500_REG_ACCEL_YOUT_H        0x3D        /**< accel yout high register */
#define MPU6500_REG_ACCEL_YOUT_L        0x3E        /**< accel yout low register */
#define MPU6500_REG_ACCEL_ZOUT_H        0x3F        /**< accel zout high register */
#define MPU6500_REG_ACCEL_ZOUT_L        0x40        /**< accel zout low register */
#define MPU6500_REG_TEMP_OUT_H          0x41        /**< temp high register */
#define MPU6500_REG_TEMP_OUT_L          0x42        /**< temp low register */
#define MPU6500_REG_GYRO_XOUT_H         0x43        /**< gyro xout high register */
#define MPU6500_REG_GYRO_XOUT_L         0x44        /**< gyro xout low register */
#define MPU6500_REG_GYRO_YOUT_H         0x45        /**< gyro yout high register */
#define MPU6500_REG_GYRO_YOUT_L         0x46        /**< gyro yout low register */
#define MPU6500_REG_GYRO_ZOUT_H         0x47        /**< gyro zout high register */
#define MPU6500_REG_GYRO_ZOUT_L         0x48        /**< gyro zout low register */
#define MPU6500_REG_SIGNAL_PATH_RESET   0x68        /**< signal path reset register */
#define MPU6500_REG_MOT_DETECT_CTRL     0x69        /**< motion detect ctrl register */
#define MPU6500_REG_USER_CTRL           0x6A        /**< user ctrl register */
#define MPU6500_REG_PWR_MGMT_1          0x6B        /**< power management 1 register */
#define MPU6500_REG_PWR_MGMT_2          0x6C        /**< power management 2 register */
#define MPU6500_REG_BANK_SEL            0x6D        /**< bank sel register */
#define MPU6500_REG_MEM                 0x6F        /**< memory register */
#define MPU6500_REG_PROGRAM_START       0x70        /**< program start register */
#define MPU6500_REG_FIFO_COUNTH         0x72        /**< fifo count high threshold register */
#define MPU6500_REG_FIFO_COUNTL         0x73        /**< fifo count low threshold register */
#define MPU6500_REG_R_W                 0x74        /**< fifo read write data register */
#define MPU6500_REG_WHO_AM_I            0x75        /**< who am I register */
#define MPU6500_REG_XA_OFFSET_H         0x77        /**< accel offset x high register */
#define MPU6500_REG_XA_OFFSET_L         0x78        /**< accel offset x low register */
#define MPU6500_REG_YA_OFFSET_H         0x7A        /**< accel offset y high register */
#define MPU6500_REG_YA_OFFSET_L         0x7B        /**< accel offset y low register */
#define MPU6500_REG_ZA_OFFSET_H         0x7D        /**< accel offset z high register */
#define MPU6500_REG_ZA_OFFSET_L         0x7E        /**< accel offset z low register */
#endif
