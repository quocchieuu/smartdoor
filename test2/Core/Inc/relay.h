#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f1xx_hal.h"   // đổi theo dòng chip của bạn

// Relay trạng thái: 0 = đóng, 1 = mở
extern uint8_t relay_state;

void relay_init(void);
void relay_on(void);
void relay_off(void);
void relay_update(uint8_t state);

#endif
