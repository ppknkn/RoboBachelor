/* SPDX-License-Identifier: GPL-3.0-only */
#include "leg_kinematics.h"
#include <math.h>

/*
 * 四杆轮腿机构运动学
 *
 * 机械结构：
 * - 两条大腿（前、后）由电机直接驱动，长度均为L1
 * - 两条小腿通过四连杆机构连接，长度均为L2
 * - 轮子安装在小腿末端，半径R
 *
 * 坐标系：
 * - 原点：髋关节中心
 * - x轴：水平向前为正
 * - z轴：竖直向下为正
 *
 * 角度约定：
 * - theta_front: 前大腿角度，竖直向下为0，向前为正
 * - theta_back:  后大腿角度，竖直向下为0，向后为负
 *
 * 等效模型：
 * - l:   等效腿长（髋关节到轮心距离）
 * - phi: 等效腿角（竖直向下为0，向前为正）
 */
/*角度归一化到[-pi, pi) */
float wrap_to_pi(float angle)
{
  
    while (angle > 3.1415926f)  angle -= 2.0f * 3.1415926f;
    while (angle < -3.1415926f) angle += 2.0f * 3.1415926f;
    return angle;
}

/* 正运动学: 前后大腿角度 -> 等效腿长/腿角 */
void leg_fk(float theta_front, float theta_back, leg_state_t *out)
{
    out->theta_front = theta_front;
    out->theta_back  = theta_back;
    out->x = 0.0f;
    out->z = 0.0f;
    out->l = 0.0f;
    out->phi = 0.0f; /* Deterministic output on degenerate geometry. */
    /* 1) 两个大腿末端坐标 */
    float x_front = L1 * sinf(theta_front);
    float z_front = L1 * cosf(theta_front);
    float x_back  = L1 * sinf(theta_back);
    float z_back  = L1 * cosf(theta_back);
    /* 2) 两圆交点法求轮心 */
    float dx = x_back - x_front;
    float dz = z_back - z_front;
    float d2 = dx * dx + dz * dz;
    float d  = sqrtf(d2);
    /* 两圆无交点/退化 */
    if (d < EPS_F)
    {
        return;
    }
    if (d > 2.0f * L2 + EPS_F)
    {
        return;
    }
    /* 浮点误差保护 */
    float half_d = 0.5f * d;
    float h2 = L2 * L2 - half_d * half_d;
    if (h2 < 0.0f)
    {
        if (h2 > -1e-5f)
            h2 = 0.0f;
        else
            return;
    }
    float h = sqrtf(h2);
    /* 中点 */
    float mx = 0.5f * (x_front + x_back);
    float mz = 0.5f * (z_front + z_back);
    /* AB 的单位法向量 */
    float nx = -dz / d;
    float nz =  dx / d;
    /* 两个候选交点 */
    float x1 = mx + h * nx;
    float z1 = mz + h * nz;
    float x2 = mx - h * nx;
    float z2 = mz - h * nz;
    /* 4) 选“更靠下”的那个点作为轮心 */
    if (fabsf(z1) > fabsf(z2))
    {
        out->x = x1;
        out->z = z1;
    }
    else
    {
        out->x = x2;
        out->z = z2;
    }
    /* 5) 腿长：轮心到髋关节原点的距离 */
    out->l = sqrtf(out->x * out->x + out->z * out->z);
    out->phi = atan2f(out->x, out->z);
}
/* 逆运动学: 期望腿长/腿角 -> 前后大腿角度 */
int leg_ik(float l_des, float phi_des, float *theta_front, float *theta_back)
{
    if (theta_front == 0 || theta_back == 0)
        return -1;
    /* 1) 基本合法性检查 */
    if (l_des < 0.0f)
        return -1;
    /* 2) 极坐标 -> 轮心坐标
     * 向下为正:
     * x = l * sin(phi)
     * z = l * cos(phi)
     */
    float x = 0.0f;
    float z = l_des;
    /* 3) 求 O(0,0) 与 P(x,z) 两圆交点
     * 圆1: 半径 L1
     * 圆2: 半径 L2
     * 交点即前/后大腿末端
     */
    float d2 = x * x + z * z;
    float d  = sqrtf(d2);
    /* 工作空间检查：
     * 两圆有交点的条件:
     * |L1 - L2| <= d <= L1 + L2
     */
    if (d < fabsf(L1 - L2) - EPS_F || d > (L1 + L2) + EPS_F)
        return -1;
    /* 轮心恰好在原点附近会退化 */
    if (d < EPS_F)
        return -1;
    /* 沿 OP 方向到交点中线的距离 */
    float a = (L1 * L1 - L2 * L2 + d2) / (2.0f * d);
    /* 交点偏移高度 */
    float h2 = L1 * L1 - a * a;
    if (h2 < 0.0f)
    {
        if (h2 > -1e-5f)
            h2 = 0.0f;
        else
            return -1;
    }
    float h = sqrtf(h2);
    /* OP单位向量 */
    float ex = x / d;
    float ez = z / d;
    /* 与OP垂直的单位向量
     * 取右法向量 / 左法向量都可以，后面靠x大小区分前后点
     */
    float nx = -ez;
    float nz =  ex;
    /* 交点中点 */
    float mx = a * ex;
    float mz = a * ez;
    /* 两个交点 */
    float x1 = mx + h * nx;
    float z1 = mz + h * nz;
    float x2 = mx - h * nx;
    float z2 = mz - h * nz;
    /* 4) 前/后大腿末端区分
     * 前大腿: x更大
     * 后大腿: x更小
     */
    float x_front_end, z_front_end;
    float x_back_end,  z_back_end;
    if (x1 >= x2)
    {
        x_front_end = x1;
        z_front_end = z1;
        x_back_end  = x2;
        z_back_end  = z2;
    }
    else
    {
        x_front_end = x2;
        z_front_end = z2;
        x_back_end  = x1;
        z_back_end  = z1;
    }
    /* 5) 大腿末端坐标 -> 大腿角
     * 因为:
     * x = L1*sin(theta)
     * z = L1*cos(theta)
     * 所以:
     * theta = atan2(x, z)
     */
    float tf = atan2f(x_front_end, z_front_end) + phi_des;
    float tb = atan2f(x_back_end,  z_back_end) + phi_des;
    tf = wrap_to_pi(tf);
    tb = wrap_to_pi(tb);
    /* 6) 机械约束检查 */
    float delta;
    if (tf < 0.0f && tb > 0.0f){
        delta = 2*PI + tf - tb;
    }
    else delta = tf - tb;
    if (delta < MIN_DELTA || delta > MAX_DELTA)
        return -1;
    *theta_front = tf;
    *theta_back  = tb;
    return 0;
}
