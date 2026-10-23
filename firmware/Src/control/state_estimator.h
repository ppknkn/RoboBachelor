/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __STATE_ESTIMATOR_H__
#define __STATE_ESTIMATOR_H__

#include <stdint.h>

typedef enum {
    ROBOT_ESTOP = 0,   // 软件STOP状态；主循环抑制新关节目标并发送零轮力矩
    ROBOT_BALANCE,     // 起身完成过渡状态；主循环随后转入行驶
    ROBOT_DRIVE,       // 行驶模式（全功能开启）
    ROBOT_STANDUP      // 起身模式（慢慢起身，拒绝遥控器位移指令）
} RobotState_e;

typedef struct {
    /* 机体姿态 (Pitch轴 - 用于直立平衡) */
    float theta;      /* pitch角 (rad), 前倾为正, 取自 imu.rol */
    float dtheta;     /* pitch角速度 (rad/s), 取自 imu.wx */
    /* 机体姿态 (Yaw轴 - 用于转向控制) */
    float yaw;        /* yaw角 (rad), 左转/右转, 取自 imu.yaw */
    float dyaw;       /* yaw角速度 (rad/s), 取自 imu.wz */
    /* 左腿 */
    float l_L;        /* 腿长 (mm) */
    float phi_L;      /* 腿角 (rad), 前倾为正 */
    float dl_L;       /* 腿长速度 (mm/s) */
    float dphi_L;     /* 腿角速度 (rad/s) */
    /* 右腿 */
    float l_R;
    float phi_R;
    float dl_R;
    float dphi_R;
    /* 轮子 */
    float v_wheel_L;  /* 左轮线速度 (m/s), main.c 将 rpm 换算后写入 */
    float v_wheel_R;
    /* 顶层逻辑状态 */
    RobotState_e robot_state; /* 机器人状态 */
    /* 控制目标值 */
    float target_v;       /* 目标前进线速度 (m/s) */
    float target_turn;    /* 目标转向角速度 (rad/s) */
    float target_leg_len; /* 目标腿长 (mm) */

    /* 定心 — 位置保持 */
    float enc_accum_L;     /* 左轮 unwrap 累计角度 (deg) */
    float enc_accum_R;     /* 右轮 unwrap 累计角度 (deg) */
    float enc_prev_L;      /* 上周期左轮 raw position (deg) */
    float enc_prev_R;      /* 上周期右轮 raw position (deg) */
    float pos_x;           /* 前进位移 (m) */
    float pos_integral;    /* 积分项已乘 Ki，单位 m/s */
    uint8_t pos_initialized; /* 位移零点已标定 */
    uint8_t pos_hold_active; /* 定心使能 */

} robot_state_t;

extern robot_state_t g_state;

void state_estimator_update(float dt);
void robot_logic_update(void);  // 根据遥控器输入和当前状态更新robot_state和控制目标  

#endif
