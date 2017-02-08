/*
 * OBD2CAN firmware
 *
 * Copyright (C) 2016 Autosport Labs
 *
 * This file is part of the Race Capture firmware suite
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should
 * have received a copy of the GNU General Public License along with
 * this code. If not, see <http://www.gnu.org/licenses/>.
 */

#include "system_CAN.h"
#include "logging.h"
#include "system_serial.h"
#include "settings.h"
#include "system.h"
#include "stm32f042x6.h"
#include "shiftx2_api.h"

#define _LOG_PFX "SYS_CAN:     "

/*
 * 500K baud; 36MHz clock
 */
static const CANConfig cancfg = {
    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP | CAN_MCR_NART,
    CAN_BTR_SJW(1) |
    CAN_BTR_TS1(11) | CAN_BTR_TS2(2) | CAN_BTR_BRP(5)
};

/* Initialize our CAN peripheral */
void system_can_init(void)
{
    // Remap PA11-12 to PA9-10 for CAN
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;

    /* CAN RX.       */
    palSetPadMode(GPIOA, 11, PAL_STM32_MODE_ALTERNATE | PAL_STM32_ALTERNATE(4));
    /* CAN TX.       */
    palSetPadMode(GPIOA, 12, PAL_STM32_MODE_ALTERNATE | PAL_STM32_ALTERNATE(4));

    /* Activates the CAN driver */
    canStart(&CAND1, &cancfg);
}


/*
 * Dispatch an incoming CAN message
 */
void dispatch_can_rx(CANRxFrame *rx_msg)
{
    uint8_t can_id_type = rx_msg->IDE;

    switch (can_id_type) {
    case CAN_IDE_EXT:
        /* Process Extended CAN IDs */
    {
        switch (rx_msg->EID) {
        case API_SET_CONFIG_GROUP_1:
            api_set_config_group_1(rx_msg);
            break;
        case API_SET_DISCRETE_LED:
            api_set_discrete_led(rx_msg);
            break;
        case API_SET_ALERT_LED:
            api_set_alert_led(rx_msg);
            break;
        case API_SET_ALERT_THRESHOLD:
            api_set_alert_threshold(rx_msg);
            break;
        case API_SET_CURRENT_ALERT_VALUE:
            api_set_current_alert_value(rx_msg);
            break;
        case API_CONFIG_LINEAR_GRAPH:
            api_config_linear_graph(rx_msg);
            break;
        case API_SET_LINEAR_THRESHOLD:
            api_set_linear_threshold(rx_msg);
            break;
        case API_SET_CURRENT_LINEAR_GRAPH_VALUE:
            api_set_current_linear_graph_value(rx_msg);
            break;
        default:
            break;
        }
        break;
    }
    break;

    case CAN_IDE_STD:
        /* Process Standard CAN IDs */
    {
        switch (rx_msg->SID) {
        default:
            break;
        }
    }
    break;
    }
}

/* Main worker for receiving CAN messages */
void can_worker(void)
{
    event_listener_t el;
    CANRxFrame rx_msg;
    chRegSetThreadName("CAN receiver");
    chEvtRegister(&CAND1.rxfull_event, &el, 0);
    while(!chThdShouldTerminateX()) {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(10)) == 0)
            continue;
        while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rx_msg, TIME_IMMEDIATE) == MSG_OK) {
            /* Process message.*/
            log_CAN_rx_message(_LOG_PFX, &rx_msg);
            dispatch_can_rx(&rx_msg);
        }
    }
    chEvtUnregister(&CAND1.rxfull_event, &el);
}

/* Prepare a CAN message with the specified CAN ID and type */
void prepare_can_tx_message(CANTxFrame *tx_frame, uint8_t can_id_type, uint32_t can_id)
{
    tx_frame->IDE = can_id_type;
    if (can_id_type == CAN_IDE_EXT) {
        tx_frame->EID = can_id;
    } else {
        tx_frame->SID = can_id;
    }
    tx_frame->RTR = CAN_RTR_DATA;
    tx_frame->DLC = 8;
    tx_frame->data8[0] = 0x55;
    tx_frame->data8[1] = 0x55;
    tx_frame->data8[2] = 0x55;
    tx_frame->data8[3] = 0x55;
    tx_frame->data8[4] = 0x55;
    tx_frame->data8[5] = 0x55;
    tx_frame->data8[6] = 0x55;
    tx_frame->data8[7] = 0x55;

}
