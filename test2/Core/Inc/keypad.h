#ifndef __KEYPAD_H__
#define __KEYPAD_H__
#define PASSWORD_LENGTH 4
#include "main.h"

#include "stm32f1xx_hal.h"

void keypad_init(void);
char keypad_scan(void);
void process_keypad(uint8_t key);
extern char entered_password[PASSWORD_LENGTH + 1];  // 🔹 cho file khác dùng

#endif
