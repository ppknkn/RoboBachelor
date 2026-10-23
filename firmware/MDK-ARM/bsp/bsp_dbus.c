/* SPDX-License-Identifier: GPL-3.0-only */
#include "bsp_dbus.h"
#include "usart.h"
#include <math.h>
#include <stdlib.h>

#define RC_DEADZONE 10       // 摇杆死区
#define RC_FILTER_ALPHA 0.8f // 摇杆低通滤波系数
#define SW_FILTER_FRAMES 3   // 拨杆消抖帧数

// 接收缓冲区开大一点，防止越界 (天地飞SBUS可能长达25字节，开到50绝对安全)
#define DBUS_RX_BUF_SIZE 50
static uint8_t dbus_buf[DBUS_RX_BUF_SIZE];

dbus_data_t g_rc = {0};
extern UART_HandleTypeDef huart1;

void bsp_dbus_init(void)
{
    // 弃用普通的 Receive_DMA，改用带有空闲中断的 ReceiveToIdle_DMA
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dbus_buf, DBUS_RX_BUF_SIZE);
}

/*
 * 关键点：使用 RxEventCallback 替代 RxCpltCallback
 * 当收到完整的一帧数据后（由于帧间有几毫秒的空闲），串口产生 IDLE 中断，回调此函数
 */
void dbus_uart_rx_callback(uint16_t Size)
{

    int head_idx = -1;
    // 遍历收到的数据，由于大疆协议有效载荷是18字节，所以 i 必须小于 Size - 17
    for (int i = 0; i <= Size - 18; i++)
    {
        if (dbus_buf[i] == 0x0F)
        {
            head_idx = i;
            break;
        }
    }

    if (head_idx != -1)
    {
        uint8_t *pbuf = &dbus_buf[head_idx]; // 对齐指针到真正的帧头

        // 2. 数据解析
        int16_t raw_ch[4];
        raw_ch[0] = ((pbuf[1] | pbuf[2] << 8) & 0x07FF) - 1024;
        raw_ch[1] = ((pbuf[2] >> 3 | pbuf[3] << 5) & 0x07FF) - 1024;
        raw_ch[2] = ((pbuf[3] >> 6 | pbuf[4] << 2 | pbuf[5] << 10) & 0x07FF) - 1024;
        raw_ch[3] = ((pbuf[5] >> 1 | pbuf[6] << 7) & 0x07FF) - 1024;

        // 3. 极其重要的：极限值校验（防乱码爆冲）
        // 摇杆正常范围应该在 -660 到 +660 之间，容错放到 700。
        // 如果解出 800 甚至更离谱的值，说明找错了帧头(正好撞上数据段的0x0F)，直接丢弃整帧！
        if (abs(raw_ch[0]) < 700 && abs(raw_ch[1]) < 700 &&
            abs(raw_ch[2]) < 700 && abs(raw_ch[3]) < 700)
        {
            // --- 数据合法，开始滤波与死区处理 ---
            static float filter_state[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            for (int i = 0; i < 4; i++)
            {
                if (raw_ch[i] > RC_DEADZONE)
                {
                    raw_ch[i] -= RC_DEADZONE;
                }
                else if (raw_ch[i] < -RC_DEADZONE)
                {
                    raw_ch[i] += RC_DEADZONE;
                }
                else
                {
                    raw_ch[i] = 0;
                }
                filter_state[i] = RC_FILTER_ALPHA * (float)raw_ch[i] + (1.0f - RC_FILTER_ALPHA) * filter_state[i];
                g_rc.ch[i] = (int16_t)filter_state[i];
            }

            // --- 拨杆数据解析 ---
            uint8_t raw_sw1 = (pbuf[6] >> 4) & 0x03;
            uint8_t raw_sw2 = (pbuf[6] >> 6) & 0x03;

            // 两个拨杆分别进行连续帧消抖。
            static uint8_t sw1_pending = 0, sw2_pending = 0;
            static uint8_t sw1_cnt = 0, sw2_cnt = 0;
            if (raw_sw1 == sw1_pending)
            {
                if (sw1_cnt < SW_FILTER_FRAMES)
                {
                    sw1_cnt++;
                    if (sw1_cnt >= SW_FILTER_FRAMES)
                        g_rc.sw1 = raw_sw1;
                }
            }
            else
            {
                sw1_pending = raw_sw1;
                sw1_cnt = 1;
            }

            if (raw_sw2 == sw2_pending)
            {
                if (sw2_cnt < SW_FILTER_FRAMES)
                {
                    sw2_cnt++;
                    if (sw2_cnt >= SW_FILTER_FRAMES)
                        g_rc.sw2 = raw_sw2;
                }
            }
            else
            {
                sw2_pending = raw_sw2;
                sw2_cnt = 1;
            }

            g_rc.online = 1; // 标定遥控器在线
        }
    }

    // 重新开启 DMA 空闲接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dbus_buf, DBUS_RX_BUF_SIZE);
}

void dbus_uart_error_callback(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dbus_buf, DBUS_RX_BUF_SIZE);
}
