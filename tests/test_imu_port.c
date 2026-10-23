/* SPDX-License-Identifier: GPL-3.0-only */
#include "imu_port.h"
#include "spi.h"
#include <assert.h>
#include <stdio.h>
SPI_HandleTypeDef hspi5;
static uint32_t chip_select=1, tick;
static unsigned tx_calls,rx_calls,fail_call;
static uint8_t command;
void HAL_GPIO_WritePin(void *port,uint16_t pin,uint32_t value)
{ assert(port==GPIOF && pin==GPIO_PIN_6);chip_select=value; }
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *h,uint8_t *data,uint16_t n,uint32_t timeout)
{
    assert(h==&hspi5 && !chip_select && timeout==10);++tx_calls;
    if(tx_calls==1) { assert(n==1);command=data[0]; }
    return tx_calls==fail_call ? HAL_TIMEOUT : HAL_OK;
}
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *h,uint8_t *data,uint16_t n,uint32_t timeout)
{
    assert(h==&hspi5 && !chip_select && timeout==10);++rx_calls;
    while(n--) *data++=0x70;
    return fail_call==3 ? HAL_ERROR : HAL_OK;
}
uint32_t HAL_GetTick(void) { return tick; }
void HAL_Delay(uint32_t ms) { tick+=ms; }
int main(void)
{
    mpu6500_handle_t h;uint8_t data[2]={1,2};imu_port_bind(&h);
    assert(h.spi_read(0x75,data,1)==0 && command==0xf5 && chip_select && data[0]==0x70);
    tx_calls=rx_calls=0;fail_call=1;
    assert(h.spi_read(0x75,data,1)!=0 && chip_select && !rx_calls);
    tx_calls=0;fail_call=2;
    assert(h.spi_write(0x9b,data,2)!=0 && command==0x1b && chip_select);
    tx_calls=0;fail_call=3;
    assert(h.spi_read(0x75,data,1)!=0 && chip_select);
    assert(h.iic_read(0,0,data,1)!=0);imu_port_delay(5);assert(imu_port_now()==5);
    puts("IMU HAL port: SPI read/write bits, chip select and error cleanup passed (mock only)");return 0;
}
