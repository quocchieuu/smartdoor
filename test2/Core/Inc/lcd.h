#ifndef __LCD_H
#define __LCD_H

#include "ssd1306.h"
#include "ds1307_for_stm32_hal.h"

typedef enum {
    LCD_MAIN,
    LCD_PASSWORD,
    LCD_FINGER,
    LCD_MESSAGE
} LCD_Mode_t;

extern LCD_Mode_t lcd_mode;

void LCD_UpdateScreen(void);
void LCD_ShowMessage(const char *msg, uint32_t delay_ms);
void LCD_ShowPassword(const char *pw);

#endif
