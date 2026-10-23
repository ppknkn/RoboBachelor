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


/* RoboBachelor 2026-10-23: raw/register/AUX subset. DMP, FIFO, self-test and unrelated APIs omitted; retained function bodies are upstream. */
#include "driver_mpu6500.h"
#include "driver_mpu6500_registers.h"

static uint8_t a_mpu6500_read(mpu6500_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == MPU6500_INTERFACE_IIC)                                     /* if iic interface */
    {
        if (handle->iic_read(handle->iic_addr, reg, (uint8_t *)buf, len) != 0)        /* read data */
        {
            return 1;                                                                 /* return error */
        }
        else
        {
            return 0;                                                                 /* success return 0 */
        }
    }
    else                                                                              /* spi interface */
    {
        if (handle->spi_read(reg | 0x80, (uint8_t *)buf, len) != 0)                   /* read data */
        {
            return 1;                                                                 /* return error */
        }
        else
        {
            return 0;                                                                 /* success return 0 */
        }
    }
}

static uint8_t a_mpu6500_write(mpu6500_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->iic_spi == MPU6500_INTERFACE_IIC)                                      /* if iic interface */
    {
        if (handle->iic_write(handle->iic_addr, reg, (uint8_t *)buf, len) != 0)        /* write data */
        {
            return 1;                                                                  /* return error */
        }
        else
        {
            return 0;                                                                  /* success return 0 */
        }
    }
    else                                                                               /* spi interface */
    {
        if (handle->spi_write(reg & (~0x80), (uint8_t *)buf, len) != 0)                /* write data */
        {
            return 1;                                                                  /* return error */
        }
        else
        {
            return 0;                                                                  /* success return 0 */
        }
    }
}

static uint8_t a_mpu6500_deinit(mpu6500_handle_t *handle)
{
    if (handle->iic_spi == MPU6500_INTERFACE_IIC)        /* if iic interface */
    {
        if (handle->iic_deinit() != 0)                   /* iic deinit */
        {
            return 1;                                    /* return error */
        }
        else
        {
            return 0;                                    /* success return 0 */
        }
    }
    else
    {
        if (handle->spi_deinit() != 0)                   /* if spi interface */
        {
            return 1;                                    /* return error */
        }
        else
        {
            return 0;                                    /* success return 0 */
        }
    }
}

uint8_t mpu6500_init(mpu6500_handle_t *handle)
{
    uint8_t res, prev;
    uint32_t timeout;
  
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->debug_print == NULL)                                                /* check debug_print */
    {
        return 3;                                                                   /* return error */
    }
    if (handle->iic_init == NULL)                                                   /* check iic_init */
    {
        handle->debug_print("mpu6500: iic_init is null.\n");                        /* iic_init is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->iic_deinit == NULL)                                                 /* check iic_deinit */
    {
        handle->debug_print("mpu6500: iic_deinit is null.\n");                      /* iic_deinit is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->iic_read == NULL)                                                   /* check iic_read */
    {
        handle->debug_print("mpu6500: iic_read is null.\n");                        /* iic_read is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->iic_write == NULL)                                                  /* check iic_write */
    {
        handle->debug_print("mpu6500: iic_write is null.\n");                       /* iic_write is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_init == NULL)                                                   /* check spi_init */
    {
        handle->debug_print("mpu6500: spi_init is null.\n");                        /* spi_init is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_deinit == NULL)                                                 /* check spi_deinit */
    {
        handle->debug_print("mpu6500: spi_deinit is null.\n");                      /* spi_deinit is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_read == NULL)                                                   /* check spi_read */
    {
        handle->debug_print("mpu6500: spi_read is null.\n");                        /* spi_read is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->spi_write == NULL)                                                  /* check spi_write */
    {
        handle->debug_print("mpu6500: spi_write is null.\n");                       /* spi_write is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->delay_ms == NULL)                                                   /* check delay_ms */
    {
        handle->debug_print("mpu6500: delay_ms is null.\n");                        /* delay_ms is null */
       
        return 3;                                                                   /* return error */
    }
    if (handle->receive_callback == NULL)                                           /* check receive_callback */
    {
        handle->debug_print("mpu6500: receive_callback is null.\n");                /* receive_callback is null */
       
        return 3;                                                                   /* return error */
    }
    
    if (handle->iic_spi == MPU6500_INTERFACE_IIC)                                   /* if iic interface */
    {
        res = handle->iic_init();                                                   /* iic init */
        if (res != 0)                                                               /* check the result */
        {
            handle->debug_print("mpu6500: iic init failed.\n");                     /* iic init failed */
           
            return 1;                                                               /* return error */
        }
    }
    else                                                                            /* if spi interface */
    {
        res = handle->spi_init();                                                   /* spi init */
        if (res != 0)                                                               /* check the result */
        {
            handle->debug_print("mpu6500: spi init failed.\n");                     /* spi init failed */
           
            return 1;                                                               /* return error */
        }
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_WHO_AM_I, &prev, 1);                   /* read who am I */
    if (res != 0)                                                                   /* check the result */
    {
        handle->debug_print("mpu6500: read who am i failed.\n");                    /* read who am I failed */
        (void)a_mpu6500_deinit(handle);                                             /* iic or spi deinit */
        
        return 5;                                                                   /* return error */
    }
    if (prev != 0x70)                                                               /* check the id */
    {
        handle->debug_print("mpu6500: id is invalid.\n");                           /* id is invalid */
        (void)a_mpu6500_deinit(handle);                                             /* iic or spi deinit */
        
        return 5;                                                                   /* return error */
    }
    
    prev = 1 << 7;                                                                  /* reset the device */
    res = a_mpu6500_write(handle, MPU6500_REG_PWR_MGMT_1, &prev, 1);                /* write pwr mgmt 1 */
    if (res != 0)                                                                   /* check the result */
    {
        handle->debug_print("mpu6500: write pwr mgmt 1 failed.\n");                 /* write pwr mgmt 1 failed */
        (void)a_mpu6500_deinit(handle);                                             /* iic or spi deinit */
        
        return 4;                                                                   /* return error */
    }
    handle->delay_ms(10);                                                           /* delay 10 ms */
    timeout = 100;                                                                  /* set the timeout 1000 ms */
    while (timeout != 0)                                                            /* check the timeout */
    {
        res = a_mpu6500_read(handle, MPU6500_REG_PWR_MGMT_1, &prev, 1);             /* read pwr mgmt 1 */
        if (res != 0)                                                               /* check the result */
        {
            handle->debug_print("mpu6500: read pwr mgmt 1 failed.\n");              /* read pwr mgmt 1 failed */
            (void)a_mpu6500_deinit(handle);                                         /* iic or spi deinit */
            
            return 4;                                                               /* return error */
        }
        if ((prev & (1 << 7)) == 0)                                                 /* check the result */
        {
            handle->inited = 1;                                                     /* flag the inited bit */
            handle->dmp_inited = 0;                                                 /* flag closed */
            
            return 0;                                                               /* success return 0 */
        }
        handle->delay_ms(10);                                                       /* delay 10 ms */
        timeout--;                                                                  /* timeout-- */
    }
    
    handle->debug_print("mpu6500: reset failed.\n");                                /* reset failed */
   
    return 4;                                                                       /* return error */
}

uint8_t mpu6500_get_reg(mpu6500_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                 /* check handle */
    {
        return 2;                                       /* return error */
    }
    if (handle->inited != 1)                            /* check handle initialization */
    {
        return 3;                                       /* return error */
    }
    
    return a_mpu6500_read(handle, reg, buf, len);       /* read data */
}

uint8_t mpu6500_set_reg(mpu6500_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                  /* check handle */
    {
        return 2;                                        /* return error */
    }
    if (handle->inited != 1)                             /* check handle initialization */
    {
        return 3;                                        /* return error */
    }
    
    return a_mpu6500_write(handle, reg, buf, len);       /* write data */
}

uint8_t mpu6500_set_iic_master(mpu6500_handle_t *handle, mpu6500_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                              /* check handle */
    {
        return 2;                                                                    /* return error */
    }
    if (handle->inited != 1)                                                         /* check handle initialization */
    {
        return 3;                                                                    /* return error */
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_USER_CTRL, (uint8_t *)&prev, 1);        /* read config */
    if (res != 0)                                                                    /* check result */
    {
        handle->debug_print("mpu6500: read user ctrl failed.\n");                    /* read user ctrl failed */
       
        return 1;                                                                    /* return error */
    }
    prev &= ~(1 << 5);                                                               /* clear config */
    prev |= enable << 5;                                                             /* set config */
    res = a_mpu6500_write(handle, MPU6500_REG_USER_CTRL, (uint8_t *)&prev, 1);       /* write config */
    if (res != 0)                                                                    /* check result */
    {
        handle->debug_print("mpu6500: write user ctrl failed.\n");                   /* write user ctrl failed */
       
        return 1;                                                                    /* return error */
    }
    
    return 0;                                                                        /* success return 0 */
}

uint8_t mpu6500_set_iic_clock(mpu6500_handle_t *handle, mpu6500_iic_clock_t clk)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                  /* check handle */
    {
        return 2;                                                                        /* return error */
    }
    if (handle->inited != 1)                                                             /* check handle initialization */
    {
        return 3;                                                                        /* return error */
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_I2C_MST_CTRL, (uint8_t *)&prev, 1);         /* read i2c mst ctrl */
    if (res != 0)                                                                        /* check result */
    {
        handle->debug_print("mpu6500: read i2c mst ctrl failed.\n");                     /* read i2c mst ctrl failed */
       
        return 1;                                                                        /* return error */
    }
    prev &= ~0xF;                                                                        /* clear the buffer */
    prev |= clk;                                                                         /* set the clock */
    res = a_mpu6500_write(handle, MPU6500_REG_I2C_MST_CTRL, (uint8_t *)&prev, 1);        /* write i2c mst ctrl */
    if (res != 0)                                                                        /* check result */
    {
        handle->debug_print("mpu6500: write i2c mst ctrl failed.\n");                    /* write i2c mst ctrl failed */
       
        return 1;                                                                        /* return error */
    }
    
    return 0;                                                                            /* success return 0 */
}

uint8_t mpu6500_set_iic_delay_enable(mpu6500_handle_t *handle, mpu6500_iic_delay_t delay, mpu6500_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                       /* check handle */
    {
        return 2;                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                  /* check handle initialization */
    {
        return 3;                                                                             /* return error */
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_I2C_MST_DELAY_CTRL, (uint8_t *)&prev, 1);        /* read i2c mst delay ctrl */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("mpu6500: read i2c mst delay ctrl failed.\n");                    /* read i2c mst delay ctrl failed */
       
        return 1;                                                                             /* return error */
    }
    prev &= ~(1 << delay);                                                                    /* clear the settings */
    prev |= enable << delay;                                                                  /* set the bool */
    res = a_mpu6500_write(handle, MPU6500_REG_I2C_MST_DELAY_CTRL, (uint8_t *)&prev, 1);       /* write i2c mst delay ctrl */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("mpu6500: write i2c mst delay ctrl failed.\n");                   /* write i2c mst delay ctrl failed */
       
        return 1;                                                                             /* return error */
    }
    
    return 0;                                                                                 /* success return 0 */
}

uint8_t mpu6500_set_iic_delay(mpu6500_handle_t *handle, uint8_t delay)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    if (delay > 0x1F)                                                                     /* check the delay */
    {
        handle->debug_print("mpu6500: delay > 0x1F.\n");                                  /* delay > 0x1F */
        
        return 4;                                                                         /* return error */
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_I2C_SLV4_CTRL, (uint8_t *)&prev, 1);         /* read i2c slv4 ctrl */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("mpu6500: read i2c slv4 ctrl failed.\n");                     /* read i2c slv4 ctrl failed */
       
        return 1;                                                                         /* return error */
    }
    prev &= ~0x1F;                                                                        /* clear the settings */
    prev |= delay;                                                                        /* set the delay */
    res = a_mpu6500_write(handle, MPU6500_REG_I2C_SLV4_CTRL, (uint8_t *)&prev, 1);        /* write i2c slv4 ctrl */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("mpu6500: write i2c slv4 ctrl failed.\n");                    /* write i2c slv4 ctrl failed */
       
        return 1;                                                                         /* return error */
    }
    
    return 0;                                                                             /* success return 0 */
}

uint8_t mpu6500_read_extern_sensor_data(mpu6500_handle_t *handle, uint8_t *data, uint8_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    if (len > 24)                                                                            /* check handle initialization */
    {
        handle->debug_print("mpu6500: len > 24.\n");                                         /* len > 24 */
        
        return 4;                                                                            /* return error */
    }
    
    res = a_mpu6500_read(handle, MPU6500_REG_EXT_SENS_DATA_00, (uint8_t *)data, len);        /* read ext sens data 00 */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("mpu6500: read ext sens data 00 failed.\n");                     /* read ext sens data 00 failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}
