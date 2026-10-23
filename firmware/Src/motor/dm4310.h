/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __DM4310_H__
#define __DM4310_H__

#include "stm32f4xx_hal.h"

/* MIT模式帧ID偏移 */
#define DM_MIT_MODE   0x000

/* 参数范围（与调试助手设定一致） */
#define DM_P_MIN   -12.5f
#define DM_P_MAX    12.5f
#define DM_V_MIN   -30.0f
#define DM_V_MAX    30.0f
#define DM_KP_MIN    0.0f
#define DM_KP_MAX  500.0f
#define DM_KD_MIN    0.0f
#define DM_KD_MAX    5.0f
#define DM_T_MIN   -10.0f
#define DM_T_MAX    10.0f

/*
 * 电机ID定义
 * 左腿：ID1(靠前大腿), ID2(靠后大腿)
 * 右腿：ID4(靠前大腿), ID3(靠后大腿)
 *
 * 正方向约定（腿向前转时）：
 *   ID1: 增大, ID2: 减小, ID3: 减小, ID4: 增大
 */
#define DM_ID_L_FRONT  1
#define DM_ID_L_BACK   2
#define DM_ID_R_BACK   3
#define DM_ID_R_FRONT  4

typedef struct {
    uint8_t  id;
    uint8_t  err;
    float    pos;   /* rad */
    float    vel;   /* rad/s */
    float    tor;   /* Nm */
    uint8_t  t_mos;
    uint8_t  t_coil;
} dm4310_fb_t;

extern dm4310_fb_t dm_fb[5]; /* index 1..4 */

void dm4310_init(void);
/* Return value confirms CAN enqueue only, not motor acknowledgement. */
HAL_StatusTypeDef dm4310_set_enabled_checked(uint8_t id, uint8_t enabled);
void dm4310_enable(uint8_t id);
void dm4310_disable(uint8_t id);
void dm4310_mit_send(uint8_t id, float pos, float vel, float kp, float kd, float tor);
void dm4310_parse_rx(uint8_t *data);

#endif /* __DM4310_H__ */
