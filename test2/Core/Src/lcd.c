#include "lcd.h"
#include "ssd1306_fonts.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "keypad.h"
#include "oled.h"

extern OLED_Mode_t oled_mode;
extern int password_index;
void LCD_UpdateScreen(void) {
    char buffer[20];

    ssd1306_Fill(Black);

    switch(oled_mode) {
        case OLED_MAIN: {
            uint8_t hour   = DS1307_GetHour();
            uint8_t minute = DS1307_GetMinute();
            uint8_t date   = DS1307_GetDate();
            uint8_t month  = DS1307_GetMonth();
            uint16_t year  = DS1307_GetYear();

            sprintf(buffer, "%02d:%02d", hour, minute);
            ssd1306_SetCursor(20, 0);
            ssd1306_WriteString(buffer, Font_16x24, White);

            sprintf(buffer, "%02d/%02d/%04d", date, month, year);
            ssd1306_SetCursor(10, 27);
            ssd1306_WriteString(buffer, Font_16x15, White);
            break;
        }

        case OLED_ENTER_PASS:
            ssd1306_SetCursor(0,0);
            ssd1306_WriteString("Nhap pass:", Font_7x10, White);

            char buf[20] = {0};
            for (int i=0; i<password_index; i++) buf[i] = '*';
            ssd1306_SetCursor(0,20);
            ssd1306_WriteString(buf, Font_11x18, White);
            break;

        case OLED_FINGER_ENROLL:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("Enroll Finger...", Font_7x10, White);
            break;

        case OLED_DELETE_FP:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("Xoa van tay...", Font_7x10, White);
            break;

        case OLED_MESSAGE:
            // Chỗ này bạn gọi display_msg() chứ không vẽ ở đây
            break;
    }

    ssd1306_UpdateScreen();
}


