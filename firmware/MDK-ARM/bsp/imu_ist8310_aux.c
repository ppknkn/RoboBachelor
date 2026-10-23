/* SPDX-License-Identifier: GPL-3.0-only */
/* New AUX transactions using MIT LibDriver transport. IST register/DRDY
 * facts: GPL C-board ist8310driver.c, 59d12b1adcd321dbf1f9e9166aef5eb95ab657bf.
 * Explicit board settings and actual differences: docs/imu-backend.md. */
#include "imu_ist8310_aux.h"
#include "imu_port.h"
#include "driver_mpu6500_registers.h"
static uint8_t put(mpu6500_handle_t *s,uint8_t r,uint8_t v)
{ return mpu6500_set_reg(s,r,&v,1); }
static uint8_t transaction(mpu6500_handle_t *s,uint8_t reg,uint8_t *value,uint8_t read)
{
    uint8_t status;
    if (put(s,MPU6500_REG_I2C_SLV4_ADDR,0x0eU|(read?0x80U:0)) ||
        put(s,MPU6500_REG_I2C_SLV4_REG,reg) ||
        (!read && put(s,MPU6500_REG_I2C_SLV4_DO,*value)) ||
        put(s,MPU6500_REG_I2C_SLV4_CTRL,0x80)) return 1;
    for(unsigned n=0;n<25;++n) {
        if(mpu6500_get_reg(s,MPU6500_REG_I2C_MST_STATUS,&status,1)||(status&0x30)) return 1;
        if(status&0x40) return read ? mpu6500_get_reg(s,MPU6500_REG_I2C_SLV4_DI,value,1) : 0;
        imu_port_delay(1);
    }
    return 1;
}
uint8_t imu_ist8310_configure(mpu6500_handle_t *s)
{
    static const uint8_t setup[][2]={{0x0a,0},{0x0b,0},{0x41,0x24},{0x42,0xc0}};
    uint8_t v=1;
    if(mpu6500_set_iic_master(s,MPU6500_BOOL_TRUE) || put(s,MPU6500_REG_I2C_MST_CTRL,0x0d) ||
       transaction(s,0x0b,&v,0)) return 1;
    imu_port_delay(10);
    if(transaction(s,0,&v,1)||v!=0x10) return 1;
    for(unsigned n=0;n<sizeof setup/sizeof setup[0];++n) {
        v=setup[n][1];
        if(transaction(s,setup[n][0],&v,0)) return 1;
        imu_port_delay(10);
        if(transaction(s,setup[n][0],&v,1)||v!=setup[n][1]) return 1;
    }
    /* SLV1 triggers conversions. SLV0 reads DRDY + XYZ (7 bytes). */
    if(put(s,MPU6500_REG_I2C_SLV1_ADDR,0x0e)||put(s,MPU6500_REG_I2C_SLV1_REG,0x0a)||
       put(s,MPU6500_REG_I2C_SLV1_DO,1)||put(s,MPU6500_REG_I2C_SLV0_ADDR,0x8e)||
       put(s,MPU6500_REG_I2C_SLV0_REG,2)||mpu6500_set_iic_delay(s,3)||
       put(s,MPU6500_REG_I2C_MST_DELAY_CTRL,3)||put(s,MPU6500_REG_I2C_SLV1_CTRL,0x81)) return 1;
    imu_port_delay(6);
    if(put(s,MPU6500_REG_I2C_SLV0_CTRL,0x87)) return 1;
    imu_port_delay(100); return 0;
}
uint8_t imu_ist8310_read(mpu6500_handle_t *s,int16_t out[3],uint8_t *fresh)
{
    uint8_t data[7],status;
    *fresh=0;
    if(mpu6500_get_reg(s,MPU6500_REG_I2C_MST_STATUS,&status,1)||(status&0x23)||
       mpu6500_read_extern_sensor_data(s,data,7)) return 1;
    *fresh=data[0]&1;
    for(unsigned a=0;a<3;++a) {
        uint16_t v=(uint16_t)(data[1+a*2]+(uint16_t)data[2+a*2]*256U);
        out[a]=(int16_t)(v<32768U?(int32_t)v:(int32_t)v-65536);
    }
    return 0;
}
