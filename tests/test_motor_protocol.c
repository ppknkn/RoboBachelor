/* SPDX-License-Identifier: GPL-3.0-only */
#include "dm4310.h"
#include "jc_motor.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

CAN_HandleTypeDef hcan1;
static CAN_TxHeaderTypeDef sent_header;
static uint8_t sent_data[8];
static unsigned sent_count;
static HAL_StatusTypeDef enqueue_result = HAL_OK;

HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *can, CAN_TxHeaderTypeDef *header,
                                     uint8_t *data, uint32_t *mailbox)
{
    assert(can == &hcan1);
    if (enqueue_result != HAL_OK) return enqueue_result;
    sent_header = *header;
    memcpy(sent_data, data, 8);
    *mailbox = 1U;
    ++sent_count;
    return HAL_OK;
}
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *c, CAN_FilterTypeDef *f)
{ (void)c; (void)f; return HAL_OK; }
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *c)
{ (void)c; return HAL_OK; }
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *c, uint32_t n)
{ (void)c; (void)n; return HAL_OK; }
void HAL_Delay(uint32_t ms) { (void)ms; }

int main(void)
{
    assert(dm4310_set_enabled_checked(0, 0) == HAL_ERROR);
    assert(sent_count == 0);
    for (uint8_t id=1; id<=4; ++id) {
        assert(dm4310_set_enabled_checked(id, 0) == HAL_OK);
        assert(sent_header.StdId == id && sent_header.DLC == 8);
        for (unsigned i=0; i<7; ++i) assert(sent_data[i] == 0xFF);
        assert(sent_data[7] == 0xFD);
        assert(dm4310_set_enabled_checked(id, 1) == HAL_OK);
        assert(sent_data[7] == 0xFC);
    }
    for (uint8_t id=5; id<=6; ++id) {
        const uint8_t idle[8] = {0x2B,0,0xA0,0,0,1,0,0};
        assert(jc_motor_set_enabled_checked(id, 0) == HAL_OK);
        assert(sent_header.StdId == 0x600U+id && sent_header.DLC == 8);
        assert(memcmp(sent_data, idle, 8) == 0);
        assert(jc_motor_set_enabled_checked(id, 1) == HAL_OK);
        assert(sent_data[2] == 0xA2 && sent_data[5] == 1);
    }
    enqueue_result = HAL_BUSY;
    unsigned previous_count = sent_count;
    assert(dm4310_set_enabled_checked(1, 0) == HAL_BUSY);
    assert(jc_motor_set_enabled_checked(5, 0) == HAL_BUSY);
    assert(sent_count == previous_count);
    puts("motor protocol: encoded frames and enqueue failures passed (mock CAN only)");
    return 0;
}
