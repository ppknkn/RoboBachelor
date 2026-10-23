/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __LEG_KINEMATICS_H__
#define __LEG_KINEMATICS_H__

#include <stdint.h>
#include <math.h>

/* 五杆机构参数 (mm) */
#define L1  83.0f   /* 大腿长度 */
#define L2  80.0f   /* 小腿长度 */
#define R   33.0f   /* 轮子半径 */

/* 机械约束: 两虚拟大腿夹角范围 (rad)，含5deg安全余量
 * 机械极限: 74.75deg ~ 116.73deg
 * 软件限位: 79.75deg(1.3919rad) ~ 111.73deg(1.9500rad)
 * 对应腿长: ~123.5mm ~ ~87.5mm */
#define MAX_DELTA 1.9500f
#define MIN_DELTA 1.3919f
#define MAX_LEG_L 123.5f
#define MIN_LEG_L  87.5f
#define IDEAL_LEG_L 102.5f  // 理想腿长

/* 数学参数 */
#ifndef PI
#define PI 3.1415926f
#endif
#ifndef EPS_F
#define EPS_F 1e-6f
#endif
#define TWO_PI (2.0f * PI)
/* 腿部状态 */
typedef struct {
    float theta_front;  /* 前大腿角度 (rad) */
    float theta_back;   /* 后大腿角度 (rad) */
    float x;            /* 轮心相对髋关节 水平位移 (mm) */
    float z;            /* 轮心相对髋关节 垂直位移 (mm), 向下为正 */
    float l;            /* 等效腿长 (mm) */
    float phi;          /* 等效腿角 (rad), 竖直向下为0 */
} leg_state_t;

/* 正运动学: 由前后大腿角度 -> 等效腿长/腿角 */
void leg_fk(float theta_front, float theta_back, leg_state_t *out);

/* 逆运动学: 由期望腿长和腿角 -> 前后大腿角度 */
/* 返回0成功, -1超出工作空间或违反机械约束 */
int  leg_ik(float l_des, float phi_des, float *theta_front, float *theta_back);
/* 角度归一化到[-pi, pi) */
float wrap_to_pi(float angle);

static inline float unwrap_near_rad(float theta_wrapped, float theta_prev)
{
    float k = roundf((theta_prev - theta_wrapped) / TWO_PI);
    return theta_wrapped + k * TWO_PI;
}
static inline float unwrap_near_deg(float theta_wrapped, float theta_prev)
{
    float k = roundf((theta_prev - theta_wrapped) / 360.0f);
    return theta_wrapped + k * 360.0f;
}

#endif
