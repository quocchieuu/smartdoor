/*
 * enroll.c
 *
 *  Created on: Aug 30, 2025
 *      Author: hp
 */
#include "stm32f1xx_hal.h"   // hoặc "stm32f4xx_hal.h" tùy dòng chip
#include <stdint.h>
#include "enroll.h"

// Định nghĩa duy nhất ở đây
EnrollState_t enroll_state = ENROLL_IDLE;
int enroll_id = -1;
uint8_t enroll_result = 0;
