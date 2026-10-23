/* SPDX-License-Identifier: GPL-3.0-only */
#include "dm4310.h"
#include "can.h"

dm4310_fb_t dm_fb[5]; /* index 1..4 */

/* ---- 内部工具函数 ---- */

static int float_to_uint(float x, float x_min, float x_max, int bits)
{
    float span = x_max - x_min;
    return (int)((x - x_min) * (float)((1 << bits) - 1) / span);
}

static float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    float span = x_max - x_min;
    return (float)x_int * span / (float)((1 << bits) - 1) + x_min;
}

/* ---- CAN 发送底层 ---- */

static HAL_StatusTypeDef can1_send(uint16_t id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef hdr = {
        .StdId = id,
        .IDE   = CAN_ID_STD,
        .RTR   = CAN_RTR_DATA,
        .DLC   = len,
    };
    uint32_t mailbox;
    return HAL_CAN_AddTxMessage(&hcan1, &hdr, data, &mailbox);
}

/* ---- 特殊指令帧 ---- */

static HAL_StatusTypeDef dm_cmd(uint8_t id, uint8_t cmd_byte)
{
    uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, cmd_byte};
    return can1_send(id + DM_MIT_MODE, data, 8);
}

/* ---- 公开接口 ---- */

void dm4310_init(void)
{
    /* CAN初始化由bsp_can_init统一完成，此处无需重复 */
}

HAL_StatusTypeDef dm4310_set_enabled_checked(uint8_t id, uint8_t enabled)
{
    if (id < 1 || id > 4) return HAL_ERROR;
    return dm_cmd(id, enabled ? 0xFC : 0xFD);
}

void dm4310_enable(uint8_t id)
{
    (void)dm4310_set_enabled_checked(id, 1);
}

void dm4310_disable(uint8_t id)
{
    (void)dm4310_set_enabled_checked(id, 0);
}

void dm4310_mit_send(uint8_t id, float pos, float vel, float kp, float kd, float tor)
{
    uint16_t p   = (uint16_t)float_to_uint(pos, DM_P_MIN,  DM_P_MAX,  16);
    uint16_t v   = (uint16_t)float_to_uint(vel, DM_V_MIN,  DM_V_MAX,  12);
    uint16_t kp_ = (uint16_t)float_to_uint(kp,  DM_KP_MIN, DM_KP_MAX, 12);
    uint16_t kd_ = (uint16_t)float_to_uint(kd,  DM_KD_MIN, DM_KD_MAX, 12);
    uint16_t t   = (uint16_t)float_to_uint(tor, DM_T_MIN,  DM_T_MAX,  12);

    uint8_t data[8];
    data[0] = p >> 8;
    data[1] = p & 0xFF;
    data[2] = v >> 4;
    data[3] = ((v & 0xF) << 4) | (kp_ >> 8);
    data[4] = kp_ & 0xFF;
    data[5] = kd_ >> 4;
    data[6] = ((kd_ & 0xF) << 4) | (t >> 8);
    data[7] = t & 0xFF;

    can1_send(id + DM_MIT_MODE, data, 8);
}

/* 在 CAN RX 中断中调用 */
void dm4310_parse_rx(uint8_t *data)
{
    uint8_t id = data[0] & 0x0F;
    if (id < 1 || id > 4) return;

    dm4310_fb_t *fb = &dm_fb[id];
    fb->id     = id;
    fb->err    = data[0] >> 4;

    uint16_t p_int = ((uint16_t)data[1] << 8) | data[2];
    uint16_t v_int = ((uint16_t)data[3] << 4) | (data[4] >> 4);
    uint16_t t_int = (uint16_t)((data[4] & 0xF) << 8) | data[5];

    fb->pos    = uint_to_float(p_int, DM_P_MIN, DM_P_MAX, 16);
    fb->vel    = uint_to_float(v_int, DM_V_MIN, DM_V_MAX, 12);
    fb->tor    = uint_to_float(t_int, DM_T_MIN, DM_T_MAX, 12);
    fb->t_mos  = data[6];
    fb->t_coil = data[7];
}
