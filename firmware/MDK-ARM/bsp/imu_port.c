/* SPDX-License-Identifier: GPL-3.0-only */
#include "imu_port.h"
#include "spi.h"
#include <string.h>
static uint8_t no_i2c(void) { return 1; }
static uint8_t no_i2c_data(uint8_t a,uint8_t r,uint8_t *b,uint16_t n)
{ (void)a; (void)r; (void)b; (void)n; return 1; }
static uint8_t spi_ready(void) { return 0; } /* MX_SPI5_Init owns lifetime. */
static void no_debug(const char *const fmt, ...) { (void)fmt; }
static void no_event(uint8_t e) { (void)e; }
static uint8_t read_spi(uint8_t reg,uint8_t *data,uint16_t n)
{
    HAL_StatusTypeDef result;
    reg |= 0x80;
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_RESET);
    result = HAL_SPI_Transmit(&hspi5,&reg,1,10);
    if (result==HAL_OK) result=HAL_SPI_Receive(&hspi5,data,n,10);
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);
    return result==HAL_OK ? 0 : 1;
}
static uint8_t write_spi(uint8_t reg,uint8_t *data,uint16_t n)
{
    HAL_StatusTypeDef result;
    reg &= 0x7f;
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_RESET);
    result=HAL_SPI_Transmit(&hspi5,&reg,1,10);
    if(result==HAL_OK) result=HAL_SPI_Transmit(&hspi5,data,n,10);
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);
    return result==HAL_OK ? 0 : 1;
}
void imu_port_bind(mpu6500_handle_t *h)
{
    memset(h,0,sizeof *h); h->iic_spi=MPU6500_INTERFACE_SPI;
    h->iic_init=h->iic_deinit=no_i2c; h->iic_read=h->iic_write=no_i2c_data;
    h->spi_init=h->spi_deinit=spi_ready; h->spi_read=read_spi; h->spi_write=write_spi;
    h->delay_ms=imu_port_delay; h->debug_print=no_debug; h->receive_callback=no_event;
}
uint32_t imu_port_now(void) { return HAL_GetTick(); }
void imu_port_delay(uint32_t ms) { HAL_Delay(ms); }
