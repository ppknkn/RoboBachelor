/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef __BALANCE_CTRL_H__
#define __BALANCE_CTRL_H__

#include "state_estimator.h"
/* PID release: speed P -> pitch PD -> wheel torque; yaw rate uses P.
 * Original gains are preserved. See docs/control.md for units and limits. */
#define BALANCE_KP     200.0f 
#define BALANCE_KD     10.0f 
#define WHEEL_KF       0.012f

/* 速度环 (串联 P-only)：输出倾角修正量 rad，不是力矩 */
#define VELOCITY_KP    0.15f  /* 速度误差 1m/s → 0.15rad 倾角修正 */
#define THETA_OFFSET   0.0f   /* 机器人前倾角补偿，单位为弧度，根据实际情况调整 */
#define LEG_L_REF      107.0f
#define LEG_POS_KP     20.0f
#define LEG_POS_KD     1.0f
    
#define TURN_KP        0.30f   /* dyaw rate → diff torque: 1rad/s偏航→0.30Nm差速 */

extern float balance_kp;
extern float balance_kd;
extern float wheel_kf;
extern float vel_kp;
extern float turn_kp;
extern float leg_pos_kp;
extern float leg_pos_kd;
extern float theta_offset;
/* Inputs: leg length in mm; angles in rad; wheel linear speed in m/s.
 * robot_state may advance from ESTOP to STANDUP and then BALANCE. */

void balance_ctrl_reset(void);

void balance_ctrl_update(
    RobotState_e *robot_state,
    float target_v, float target_turn,
    float target_leg_len,
    float l_L, float phi_L,
    float l_R, float phi_R,
    float theta, float dtheta,
    float yaw_wz,
    float wheel_v_L, float wheel_v_R,
    float *tau_fL, float *tau_bL,
    float *tau_fR, float *tau_bR,
    float *pos_fL, float *pos_bL,
    float *pos_fR, float *pos_bR,
    float *vel_fL, float *vel_bL,   /* MIT vel 前馈 */
    float *vel_fR, float *vel_bR,
    float *tau_wheel_L, float *tau_wheel_R);
    
#endif
