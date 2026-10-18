#ifndef LIQUIDCRYSTAL_I2C_H
#define LIQUIDCRYSTAL_I2C_H

#include "stm32f1xx_hal.h"

// Khai báo các hàm điều khiển LCD
void HD44780_Init(uint8_t rows);
void HD44780_Clear(void);
void HD44780_SetCursor(uint8_t row, uint8_t col);
void HD44780_PrintStr(const char *str);
void HD44780_Home(void);

#endif
