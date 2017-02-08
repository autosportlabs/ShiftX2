/*
 * shiftx2_api.h
 *
 *  Created on: Feb 4, 2017
 *      Author: brent
 */

#ifndef SHIFTX2_API_H_
#define SHIFTX2_API_H_
#include "ch.h"
#include "hal.h"
#include "system_CAN.h"

#define ALERT_THRESHOLDS 5

struct AlertThreshold {
    uint16_t threshold;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t flash_hz;
};

enum render_style {
    RENDER_STYLE_LEFT_RIGHT = 0,
    RENDER_STYLE_CENTER,
    RENDER_STYLE_RIGHT_LEFT
};

enum linear_style {
  LINEAR_STYLE_SMOOTH = 0,
  LINEAR_STYLE_STEPPED
};

/*Linear Graph Configuration */
#define LINEAR_GRAPH_THRESHOLDS 5
#define DEFAULT_LINEAR_GRAPH_COLOR_RED  0
#define DEFAULT_LINEAR_GRAPH_COLOR_GREEN  255
#define DEFAULT_LINEAR_GRAPH_COLOR_BLUE  0
#define DEFAULT_LINEAR_GRAPH_FLASH  0

struct LedFlashConfig {
    uint8_t current_state;
    uint8_t flash_hz;
};

struct LinearGraphConfig {
    enum render_style render_style;
    enum linear_style linear_style;
    uint16_t low_range;
    uint16_t high_range;
};

struct LinearGraphThreshold {
    uint8_t segment_length;
    uint16_t threshold;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t flash_hz;
};

/* API offsets */
#define SHIFTX2_CAN_BASE_ID 744389
#define API_ANNOUNCEMENT SHIFTX2_CAN_BASE_ID + 0
#define API_RESET_DEVICE SHIFTX2_CAN_BASE_ID + 1
#define API_STATS SHIFTX2_CAN_BASE_ID + 2
#define API_SET_CONFIG_GROUP_1 SHIFTX2_CAN_BASE_ID + 3

/* Configuration and Runtime */
#define API_SET_DISCRETE_LED SHIFTX2_CAN_BASE_ID + 10
#define API_SET_ALERT_LED SHIFTX2_CAN_BASE_ID + 11
#define API_SET_ALERT_THRESHOLD SHIFTX2_CAN_BASE_ID + 12
#define API_SET_CURRENT_ALERT_VALUE SHIFTX2_CAN_BASE_ID + 13
#define API_CONFIG_LINEAR_GRAPH SHIFTX2_CAN_BASE_ID + 14
#define API_SET_LINEAR_THRESHOLD SHIFTX2_CAN_BASE_ID + 15
#define API_SET_CURRENT_LINEAR_GRAPH_VALUE SHIFTX2_CAN_BASE_ID + 16


void set_brightness(uint8_t brightness);
uint8_t get_brightness(void);
struct LedFlashConfig * get_flash_config(size_t index);
void set_flash_config(size_t led_index, uint8_t flash_hz);

/* Base API functions */
void api_initialize(void);
void api_set_config_group_1(CANRxFrame *rx_msg);
void api_set_discrete_led(CANRxFrame *rx_msg);

/* Alert related functions */
void api_set_alert_led(CANRxFrame *rx_msg);
void api_set_alert_threshold(CANRxFrame *rx_msg);
void api_set_current_alert_value(CANRxFrame *rx_msg);

/* Linear graph related functions */
void api_config_linear_graph(CANRxFrame *rx_msg);
void api_set_linear_threshold(CANRxFrame *rx_msg);
void api_set_current_linear_graph_value(CANRxFrame *rx_msg);

void api_send_announcement(void);

#endif /* SHIFTX2_API_H_ */