/* SPDX-License-Identifier: GPL-3.0-only */
#include "state_estimator.h"
#include <math.h>
#include "leg_kinematics.h"
#include "dm4310.h"
#include "bsp_imu.h"
#include "bsp_dbus.h"
#include "jc_motor.h" /* for g_motor_fb, MOTOR_L_WHEEL, MOTOR_R_WHEEL */

robot_state_t g_state = {0};

extern imu_t imu;

/* 一阶低通滤波，截止频率由 alpha 决定: alpha = dt/(tau+dt) */
static float lpf(float prev, float raw, float alpha)
{
    return prev + alpha * (raw - prev);
}

void state_estimator_update(float dt)
{
    /* 1. 机体姿态：直接取IMU */
    g_state.theta  = imu.rol * 0.017453293f;  /* deg -> rad */
    g_state.dtheta = imu.wx;                  /* wx 已是 rad/s */

    g_state.yaw    = imu.yaw * 0.017453293f;  /* deg -> rad */
    g_state.dyaw   = imu.wz;                  /* wz 已是 rad/s */
    /* 2. 关节角映射（符号约定：腿向前转为正） */
    float tf_L =  dm_fb[1].pos;   /* ID1 左前 */
    float tb_L = -dm_fb[2].pos;   /* ID2 左后 */
    float tf_R =  dm_fb[4].pos;   /* ID4 右前 */
    float tb_R = -dm_fb[3].pos;   /* ID3 右后 */

    /* 3. 正运动学 */
    leg_state_t leg_L, leg_R;
    leg_fk(tf_L, tb_L, &leg_L);
    leg_fk(tf_R, tb_R, &leg_R);

    /* 4. 腿长/腿角速度：一阶差分 + 低通滤波 (tau≈5ms, 500Hz下alpha≈0.29) */
    const float alpha = 0.29f;

    float dl_L_raw   = (leg_L.l   - g_state.l_L)   / dt;
    float dphi_L_raw = (leg_L.phi - g_state.phi_L)  / dt;
    float dl_R_raw   = (leg_R.l   - g_state.l_R)    / dt;
    float dphi_R_raw = (leg_R.phi - g_state.phi_R)  / dt;

    g_state.dl_L   = lpf(g_state.dl_L,   dl_L_raw,   alpha);
    g_state.dphi_L = lpf(g_state.dphi_L, dphi_L_raw, alpha);
    g_state.dl_R   = lpf(g_state.dl_R,   dl_R_raw,   alpha);
    g_state.dphi_R = lpf(g_state.dphi_R, dphi_R_raw, alpha);

    g_state.l_L   = leg_L.l;
    g_state.phi_L = leg_L.phi;
    g_state.l_R   = leg_R.l;
    g_state.phi_R = leg_R.phi;

    /* ── 5. 编码器位移计算（带 unwrap）── */
    /* JC position: 24-bit signed, 范围 ±83886°, 全局范围 167772° */
#define ENC_FULL_RANGE  167772.0f
#define ENC_WRAP_THRESH  80000.0f  /* 2ms 内不可能跳变 80000° */
    const float DEG_TO_DIST = (3.14159265f / 180.0f) * 0.033f;

    float raw_L = g_motor_fb[MOTOR_L_WHEEL].position;
    float raw_R = g_motor_fb[MOTOR_R_WHEEL].position;

    if (!g_state.pos_initialized) {
        g_state.enc_accum_L = 0.0f;
        g_state.enc_accum_R = 0.0f;
        g_state.enc_prev_L  = raw_L;
        g_state.enc_prev_R  = raw_R;
        g_state.pos_initialized = 1;
    }

    /* Unwrap 左轮 */
    float dL = raw_L - g_state.enc_prev_L;
    if (dL >  ENC_WRAP_THRESH) dL -= ENC_FULL_RANGE;
    if (dL < -ENC_WRAP_THRESH) dL += ENC_FULL_RANGE;
    g_state.enc_accum_L += dL;
    g_state.enc_prev_L   = raw_L;

    /* Unwrap 右轮 */
    float dR = raw_R - g_state.enc_prev_R;
    if (dR >  ENC_WRAP_THRESH) dR -= ENC_FULL_RANGE;
    if (dR < -ENC_WRAP_THRESH) dR += ENC_FULL_RANGE;
    g_state.enc_accum_R += dR;
    g_state.enc_prev_R   = raw_R;

    /* 位移：左右平均 (m) */
    g_state.pos_x = (g_state.enc_accum_L + g_state.enc_accum_R) * 0.5f * DEG_TO_DIST;

    /* ── 6. 位置环 PI（定心）── */
#define POS_HOLD_KP 0.4f   /* 位置误差 1m → 0.4 m/s */
#define POS_HOLD_KI 0.02f  /* 位置积分 */

    if (g_state.pos_hold_active) {
        float pos_err = 0.0f - g_state.pos_x;  /* 目标 = 原点 */

        g_state.pos_integral += POS_HOLD_KI * pos_err * dt;
        if (g_state.pos_integral >  2.0f) g_state.pos_integral =  2.0f;
        if (g_state.pos_integral < -2.0f) g_state.pos_integral = -2.0f;

        float pos_output = POS_HOLD_KP * pos_err + g_state.pos_integral;

        if (pos_output >  0.5f) pos_output =  0.5f;
        if (pos_output < -0.5f) pos_output = -0.5f;

        g_state.target_v += pos_output;
    }
}

void robot_logic_update(void)
{
    // ==========================================
    // 1. 状态机切换 (使用左拨杆 SW1)
    // ==========================================
    if (g_rc.sw1 == 0) {
        // 拨杆在下：急停 / 锁死
        g_state.robot_state = ROBOT_ESTOP;
        g_state.target_v = g_state.target_turn = 0.0f;
        g_state.pos_hold_active = 0;
        g_state.pos_integral = 0.0f;
        return;
    }
    else {
        // 拨杆中位或上位：正常行驶流程 ESTOP → STANDUP → BALANCE → DRIVE
        if (g_state.robot_state == ROBOT_ESTOP || g_state.robot_state == ROBOT_STANDUP) {
            g_state.robot_state = ROBOT_STANDUP;
            g_state.pos_initialized = 0;
            g_state.pos_integral = 0.0f;
        } else if (g_state.robot_state == ROBOT_BALANCE) {
            g_state.robot_state = ROBOT_DRIVE;
            g_state.pos_initialized = 0;
            g_state.pos_integral = 0.0f;
        }
    }
    // ==========================================
    // 2. 遥控器通道映射 (摇杆范围 -700 到 700)
    // ==========================================
    if (g_state.robot_state == ROBOT_DRIVE) {
        // CH0 (右摇杆上下) -> 控制前进后退, 打满 ±1.5 m/s
        // 注意：ch[0] 正=摇杆上推=期望加速后退（倒车方向），符号取负
        g_state.target_v = -(g_rc.ch[0] / 700.0f) * 1.5f;

        // CH1 (左摇杆左右) -> 控制原地转向, 打满 ±5 rad/s
        g_state.target_turn = -(g_rc.ch[1] / 700.0f) * 5.0f;

        /* ── 定心使能：CH0 死区检测 ── */
        if (fabsf((float)g_rc.ch[0]) < 15) {  /* 死区 ±15（总范围 ±660） */
            g_state.pos_hold_active = 1;
        } else {
            /* 遥控激活：关闭定心，在当前位置重新标零 */
            g_state.pos_hold_active = 0;
            g_state.enc_accum_L = 0.0f;
            g_state.enc_accum_R = 0.0f;
            g_state.enc_prev_L  = g_motor_fb[MOTOR_L_WHEEL].position;
            g_state.enc_prev_R  = g_motor_fb[MOTOR_R_WHEEL].position;
            g_state.pos_x = 0.0f;
            g_state.pos_integral = 0.0f;
        }
    }
    // CH2 (左摇杆上下) -> 控制腿长伸缩 (无论行驶还是原地均可伸缩)
    // 中心 IDEAL_LEG_L=102.5, 机械上限 123.5(伸长余量21mm), 下限 87.5(缩短余量15mm)
    if (g_state.robot_state != ROBOT_ESTOP) {
        if (g_rc.ch[2] >= 0) {
            // 上拨伸长：打满 +20mm → 122.5 (接近 MAX_LEG_L=123.5)
            g_state.target_leg_len = IDEAL_LEG_L + (g_rc.ch[2] / 700.0f) * 20.0f;
        } else {
            // 下拨缩短：打满 -14mm → 88.5 (留 1mm 余量, 不触 MIN_LEG_L)
            g_state.target_leg_len = IDEAL_LEG_L + (g_rc.ch[2] / 700.0f) * 14.0f;
        }
        // 硬限幅保护
        if (g_state.target_leg_len > 122.5f) g_state.target_leg_len = 122.5f;
        if (g_state.target_leg_len <  88.5f) g_state.target_leg_len =  88.5f;
    }
}
