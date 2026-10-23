/* SPDX-License-Identifier: GPL-3.0-only */
/* Real backend + LibDriver + Mahony, fake register bus only. No hardware. */
#include "bsp_imu.h"
#include "imu_port.h"
#include "driver_mpu6500_registers.h"
#include "MahonyAHRS.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static uint8_t regfile[256], magfile[256], raw[14], ext[7];
static uint32_t clock_ms;
static uint8_t bus_fault, aux_nack, aux_stall;
static unsigned sample_reads;
static uint8_t ok(void) { return 0; }
static void silent(const char *const fmt, ...) { (void)fmt; }
static void event(uint8_t e) { (void)e; }
uint32_t imu_port_now(void) { return clock_ms; }
void imu_port_delay(uint32_t ms) { clock_ms += ms; }

static uint8_t read_reg(uint8_t address,uint8_t *out,uint16_t count)
{
    address &= 0x7f; /* LibDriver SPI callback receives the wire read bit. */
    if(bus_fault) return 1;
    if(address==MPU6500_REG_ACCEL_XOUT_H && count==14) { memcpy(out,raw,14); ++sample_reads; }
    else if(address==MPU6500_REG_EXT_SENS_DATA_00 && count==7) memcpy(out,ext,7);
    else memcpy(out,regfile+address,count);
    return 0;
}
static uint8_t write_reg(uint8_t address,uint8_t *in,uint16_t count)
{
    address &= 0x7f;
    if(bus_fault) return 1;
    memcpy(regfile+address,in,count);
    if(address==MPU6500_REG_PWR_MGMT_1 && (in[0]&0x80)) regfile[address]=0; /* reset completes */
    if(address==MPU6500_REG_I2C_SLV4_CTRL && (in[0]&0x80)) {
        uint8_t index=regfile[MPU6500_REG_I2C_SLV4_REG];
        if(aux_stall) regfile[MPU6500_REG_I2C_MST_STATUS]=0;
        else if(aux_nack) regfile[MPU6500_REG_I2C_MST_STATUS]=0x10;
        else {
            regfile[MPU6500_REG_I2C_MST_STATUS]=0x40;
            if(regfile[MPU6500_REG_I2C_SLV4_ADDR]&0x80) regfile[MPU6500_REG_I2C_SLV4_DI]=magfile[index];
            else magfile[index]=regfile[MPU6500_REG_I2C_SLV4_DO];
        }
    }
    return 0;
}
static uint8_t unused_i2c(uint8_t a,uint8_t r,uint8_t *d,uint16_t n)
{ (void)a; (void)r; (void)d; (void)n; return 1; }
void imu_port_bind(mpu6500_handle_t *h)
{
    memset(h,0,sizeof *h);h->iic_spi=MPU6500_INTERFACE_SPI;
    h->iic_init=h->iic_deinit=h->spi_init=h->spi_deinit=ok;
    h->iic_read=h->iic_write=unused_i2c;h->spi_read=read_reg;h->spi_write=write_reg;
    h->delay_ms=imu_port_delay;h->debug_print=silent;h->receive_callback=event;
}
static void be(uint8_t *p,int16_t v) { p[0]=(uint16_t)v>>8;p[1]=(uint8_t)v; }
static void le(uint8_t *p,int16_t v) { p[0]=(uint8_t)v;p[1]=(uint16_t)v>>8; }
static void fixture(void)
{
    memset(regfile,0,sizeof regfile);memset(magfile,0,sizeof magfile);
    memset(raw,0,sizeof raw);memset(ext,0,sizeof ext);
    clock_ms=sample_reads=0;bus_fault=aux_nack=aux_stall=0;
    regfile[MPU6500_REG_WHO_AM_I]=0x70;magfile[0]=0x10;
    be(raw+4,4096);be(raw+8,100);be(raw+10,-50);be(raw+12,20);
    ext[0]=1;le(ext+1,500);le(ext+3,0);le(ext+5,0);
}
static void update(void)
{ clock_ms+=2;mpu_get_data();imu_ahrs_update();imu_attitude_update(); }
int main(void)
{
    fixture();assert(mpu_device_init()==0);assert(sample_reads==301);
    assert(regfile[MPU6500_REG_GYRO_CONFIG]==0x18 && regfile[MPU6500_REG_ACCEL_CONFIG]==0x10);
    assert(regfile[MPU6500_REG_CONFIG]==4 && regfile[MPU6500_REG_ACCEL_CONFIG2]==2);
    assert(regfile[MPU6500_REG_I2C_SLV0_ADDR]==0x8e && regfile[MPU6500_REG_I2C_SLV0_REG]==2);
    assert(regfile[MPU6500_REG_I2C_SLV0_CTRL]==0x87 && regfile[MPU6500_REG_I2C_SLV1_CTRL]==0x81);
    assert(magfile[0x41]==0x24 && magfile[0x42]==0xc0);
    mpu_get_data();init_quaternion();imu_attitude_update();assert(imu_is_healthy());
    assert(fabsf(imu.wx)<1e-6f && fabsf(imu.wy)<1e-6f && fabsf(imu.wz)<1e-6f);
    assert(imu.az==4096 && fabsf(imu.temp-21)<1e-6f);
    for(unsigned n=0;n<1000;++n) update();
    assert(imu_is_healthy() && fabsf(imu.rol)<0.01f && fabsf(imu.yaw)<0.01f);
    /* A new DRDY sample can have the same numeric vector; it remains valid. */
    ext[0]=0;clock_ms+=201;update();assert(!imu_is_healthy());
    ext[0]=1;update();assert(imu_is_healthy());
    bus_fault=1;update();assert(!imu_is_healthy());bus_fault=0;update();assert(imu_is_healthy());
    be(raw+8,16484);update();assert(fabsf(imu.wx-1000.0f/57.3f)<1e-5f);
    /* Negative signed raw fields and degree/sign interface from known tilt. */
    fixture();assert(!mpu_device_init());
    {
        float r=0.34906585f,p=-0.26179939f;
        be(raw,(int16_t)(-sinf(p)*4096));be(raw+2,(int16_t)(sinf(r)*cosf(p)*4096));
        be(raw+4,(int16_t)(cosf(r)*cosf(p)*4096));
        le(ext+1,(int16_t)(cosf(p)*1000));le(ext+3,(int16_t)(sinf(r)*sinf(p)*1000));
        le(ext+5,(int16_t)(cosf(r)*sinf(p)*1000));
    }
    mpu_get_data();init_quaternion();imu_attitude_update();
    assert(fabsf(imu.rol-20)<0.1f && fabsf(imu.pit-15)<0.1f && fabsf(imu.yaw)<0.1f);
    /* Tick wrap and same-tick update remain bounded. */
    clock_ms=UINT32_MAX-2;mpu_get_data();init_quaternion();update();update();assert(imu_is_healthy());
    imu_ahrs_update();assert(imu_is_healthy());
    fixture();regfile[MPU6500_REG_WHO_AM_I]=0;assert(mpu_device_init()!=0 && !imu_is_healthy());
    fixture();magfile[0]=0;assert(mpu_device_init()!=0);
    fixture();aux_nack=1;assert(mpu_device_init()!=0);
    fixture();aux_stall=1;assert(mpu_device_init()!=0 && clock_ms<1000);
    fixture();ext[0]=0;assert(mpu_device_init()!=0); /* absent magnetic sample is not nine-axis success */
    fixture();assert(!mpu_device_init());memset(raw,0,sizeof raw);mpu_get_data();init_quaternion();assert(!imu_is_healthy());
    /* Pure donor core: known 1rad/s yaw for one second, no magnetic input. */
    {
        float q[4]={1,0,0,0};MahonyReset();MahonySetPeriod(0.002f);
        for(unsigned n=0;n<500;++n) MahonyAHRSupdate(q,0,0,1,0,0,1,0,0,0);
        assert(fabsf(2*atan2f(q[3],q[0])-1)<0.001f);
        assert(fabsf(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]-1)<1e-5f);
    }
    puts("IMU backend: register mock, calibration, AUX errors, units, tilt, time and fusion passed (no hardware)");
    return 0;
}
