/* SPDX-License-Identifier: GPL-3.0-only */
#include "balance_ctrl.h"
#include "leg_kinematics.h"
#include "dm4310.h"
#include "bsp_dbus.h"
#include "state_estimator.h"
#include <math.h>
#include "usart.h"
float balance_kp = BALANCE_KP;
float balance_kd = BALANCE_KD;
float wheel_kf   = WHEEL_KF;
float vel_kp     = VELOCITY_KP;   /* 速度→倾角增益 rad·s/m */
float turn_kp    = TURN_KP;
float leg_pos_kp = LEG_POS_KP;
float leg_pos_kd = LEG_POS_KD;
float theta_offset = THETA_OFFSET; // 机体前倾角补偿，单位为弧度，根据实际情况调整

static RobotState_e last_state = ROBOT_ESTOP;
static float filtered_v;
static float target_fL, target_bL, target_fR, target_bR;
static uint8_t targets_valid;

void balance_ctrl_reset(void)
{
    last_state = ROBOT_ESTOP;
    filtered_v = 0.0f;
    targets_valid = 0;
}

/* The PID controller does not use a VMC Jacobian. */
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
    float *vel_fL, float *vel_bL,
    float *vel_fR, float *vel_bR,
    float *tau_wheel_L, float *tau_wheel_R)
{
    /* Geometry feedback is reserved in this interface; PID uses target-leg IK. */
    (void)l_L; (void)phi_L; (void)l_R; (void)phi_R;

    // === 静态变量定义区 ===

    // vel 默认清零（非起身状态）
    *vel_fL = 0; *vel_bL = 0; *vel_fR = 0; *vel_bR = 0;

    // ==========================================
    // 状态 ROBOT_ESTOP: 清零输出；main 的禁能门控禁止发送运动目标
    // ==========================================
    if (*robot_state == ROBOT_ESTOP)
    {
        *tau_wheel_L = 0.0f; *tau_wheel_R = 0.0f;
        *tau_fL = 0.0f; *tau_bL = 0.0f;
        *tau_fR = 0.0f; *tau_bR = 0.0f;

        *pos_fL = 0.0f; *pos_bL = 0.0f;
        *pos_fR = 0.0f; *pos_bR = 0.0f;
        targets_valid = 0;
        filtered_v = 0.0f;

        last_state = ROBOT_ESTOP;
        return;
    }
    if (last_state == ROBOT_ESTOP && *robot_state != ROBOT_ESTOP) {
        filtered_v = 0.0f;
        *robot_state = ROBOT_STANDUP;
    }
    // ==========================================
    // 状态 ROBOT_STANDUP: 起身 — kp=0 纯速度控制，v_des 驱动电机匀速转动
    // ==========================================
    if (*robot_state == ROBOT_STANDUP)
    {
        const float RAMP_VEL = 3.2f;     // 最大起身速度 rad/s (改这里调速)

        // 1. 首次进入：算一次目标 IK
        if (!targets_valid) {
            float tf, tb;
            if (leg_ik(target_leg_len, 0.0f, &tf, &tb) != 0) {
                tf = 0.716f; tb = -0.716f;
            }
            target_fL = tf;   target_bL = -tb;
            target_fR = tf;   target_bR = -tb;
            targets_valid = 1;
        }

        // 2. 读当前实际位置
        float cur_fL = dm_fb[DM_ID_L_FRONT].pos;
        float cur_bL = dm_fb[DM_ID_L_BACK].pos;
        float cur_fR = dm_fb[DM_ID_R_FRONT].pos;
        float cur_bR = dm_fb[DM_ID_R_BACK].pos;

        float err_fL = fabsf(target_fL - cur_fL);
        float err_bL = fabsf(target_bL - cur_bL);
        float err_fR = fabsf(target_fR - cur_fR);
        float err_bR = fabsf(target_bR - cur_bR);

        // 3. 找到最远电机的误差，其他电机按比例降速 → 同时到达
        float max_err = err_fL;
        if (err_bL > max_err) max_err = err_bL;
        if (err_fR > max_err) max_err = err_fR;
        if (err_bR > max_err) max_err = err_bR;

        float scale = (max_err > 0.005f) ? (RAMP_VEL / max_err) : 0.0f;

        *vel_fL = (target_fL - cur_fL) * scale;
        *vel_bL = (target_bL - cur_bL) * scale;
        *vel_fR = (target_fR - cur_fR) * scale;
        *vel_bR = (target_bR - cur_bR) * scale;

        // kp=0 时 pos 无效, 保持当前值避免切换时跳变
        *pos_fL = cur_fL;  *pos_bL = cur_bL;
        *pos_fR = cur_fR;  *pos_bR = cur_bR;

        *tau_wheel_L = 0; *tau_wheel_R = 0;
        *tau_fL = 0; *tau_bL = 0; *tau_fR = 0; *tau_bR = 0;

        // 4. 到位检测
        const float ARRIVE_THR = 0.05f;
        if (err_fL < ARRIVE_THR && err_bL < ARRIVE_THR &&
            err_fR < ARRIVE_THR && err_bR < ARRIVE_THR) {
            *robot_state = ROBOT_BALANCE;
            targets_valid = 0;
            filtered_v = 0.0f;
        }

        last_state = *robot_state;
        return;
    }
    // ==========================================
    // 正常平衡状态 (ROBOT_BALANCE)
    // ==========================================
    
    // 从急停切换时重置速度滤波状态。
    if (last_state == ROBOT_ESTOP) {
        filtered_v = 0.0f;
    }
    last_state = *robot_state;
    // 1. 速度环 (P-only → 倾角修正，串联结构)
    float avg_v = (wheel_v_L + wheel_v_R) * 0.5f;
    filtered_v = 0.7f * filtered_v + 0.3f * avg_v;

    float vel_error = target_v - filtered_v;
    float tilt_adj = vel_kp * vel_error;
    if (tilt_adj >  0.25f) tilt_adj =  0.25f;
    if (tilt_adj < -0.25f) tilt_adj = -0.25f;
    // 2. 直立环 (串联：速度环输出修正 theta_offset)
    float theta_total = theta_offset + tilt_adj;
    float F = balance_kp * (theta - theta_total) + balance_kd * dtheta;
    
    float tw = wheel_kf * F;
    if (tw >  0.48f) tw =  0.48f;
    if (tw < -0.48f) tw = -0.48f;
    
    // 3. 转向环：dyaw rate control
    //    遥控器拨杆 → target_turn (rad/s), Kp 把 rate error 映射为差速扭矩
    float turn_error = target_turn - yaw_wz;
    float turn_torque = turn_kp * (-turn_error); // 注意这里是负号，因为正的偏航速率需要负的转向扭矩来纠正
    if (turn_torque >  0.25f) turn_torque =  0.25f;
    if (turn_torque < -0.25f) turn_torque = -0.25f;
    
    // 4. 轮毂力矩合成 (转向 = 差速，左右符号相反)
    float final_L = tw + turn_torque;
    float final_R = -tw + turn_torque;
    
    if(final_L > 0.48f) final_L = 0.48f; else if(final_L < -0.48f) final_L = -0.48f;
    if(final_R > 0.48f) final_R = 0.48f; else if(final_R < -0.48f) final_R = -0.48f;
    *tau_wheel_L = final_L;
    *tau_wheel_R = final_R;
    
    // 5. 平衡时的腿长解算 (直接追踪 target_leg_len)
    float tf_ref, tb_ref;
    if (leg_ik(target_leg_len, 0.0f, &tf_ref, &tb_ref) != 0) {
        tf_ref =  0.716f;        
        tb_ref = -0.716f;
    }
    
    *pos_fL =  tf_ref; *pos_bL = -tb_ref;
    *pos_fR =  tf_ref; *pos_bR = -tb_ref;
    *tau_fL = 0; *tau_bL = 0; *tau_fR = 0; *tau_bR = 0;
}
