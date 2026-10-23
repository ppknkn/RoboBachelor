/* SPDX-License-Identifier: GPL-3.0-only */
/* Exercise actual legacy state logic; this does not validate RF decoding. */
#include "state_estimator.h"
#include "bsp_dbus.h"
#include "bsp_imu.h"
#include "dm4310.h"
#include "jc_motor.h"
#include "leg_kinematics.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
dbus_data_t g_rc;
imu_t imu;
dm4310_fb_t dm_fb[5];
jc_feedback_t g_motor_fb[7];
void leg_fk(float a,float b,leg_state_t *out)
{ (void)a;(void)b;memset(out,0,sizeof *out); }
int main(void)
{
    g_state.robot_state=ROBOT_DRIVE;g_state.target_v=1;g_state.target_turn=2;
    g_state.pos_hold_active=1;g_state.pos_integral=1;
    g_rc.sw1=0;robot_logic_update();
    assert(g_state.robot_state==ROBOT_ESTOP && g_state.target_v==0 && g_state.target_turn==0);
    assert(!g_state.pos_hold_active && g_state.pos_integral==0);
    g_rc.sw1=1;robot_logic_update();assert(g_state.robot_state==ROBOT_STANDUP);
    /* No ARM request or freshness timeout was added to this original path. */
    g_state.robot_state=ROBOT_BALANCE;robot_logic_update();assert(g_state.robot_state==ROBOT_DRIVE);
    g_rc.ch[0]=350;g_rc.ch[1]=-350;robot_logic_update();
    assert(g_state.target_v==-0.75f && g_state.target_turn==2.5f);
    g_rc.sw1=0;robot_logic_update();assert(g_state.robot_state==ROBOT_ESTOP);
    puts("Legacy STOP/state and command scales passed (no receiver or motor hardware)");return 0;
}
