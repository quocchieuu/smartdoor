#include "liquidcrystal_i2c.h"
#include "relay.h"
#define LCD_ADDR 0x4E  // Địa chỉ LCD I2C
#define LCD_BACKLIGHT 0x08

extern I2C_HandleTypeDef hi2c1;

void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_ToggleEnable(uint8_t data);

void LCD_Init() {
    HAL_Delay(50);
    LCD_SendCommand(0x33);
    LCD_SendCommand(0x32);
    LCD_SendCommand(0x28);
    LCD_SendCommand(0x0C);
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x01);
    HAL_Delay(5);
}

void LCD_Clear() {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(address);
}

void LCD_SendString(char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}


void LCD_SendCommand(uint8_t cmd) {
    uint8_t high_nibble = (cmd & 0xF0) | LCD_BACKLIGHT;
    uint8_t low_nibble  = ((cmd << 4) & 0xF0) | LCD_BACKLIGHT;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &high_nibble, 1, HAL_MAX_DELAY);
    LCD_ToggleEnable(high_nibble);
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &low_nibble, 1, HAL_MAX_DELAY);
    LCD_ToggleEnable(low_nibble);
}

void LCD_SendData(uint8_t data) {
    uint8_t high_nibble = (data & 0xF0) | LCD_BACKLIGHT | 0x01;
    uint8_t low_nibble  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | 0x01;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &high_nibble, 1, HAL_MAX_DELAY);
    LCD_ToggleEnable(high_nibble);
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &low_nibble, 1, HAL_MAX_DELAY);
    LCD_ToggleEnable(low_nibble);
}

void LCD_ToggleEnable(uint8_t data) {
    uint8_t temp = data | 0x04;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &temp, 1, HAL_MAX_DELAY);
    temp = data & ~0x04;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &temp, 1, HAL_MAX_DELAY);
}

// Biến trạng thái cửa (giả sử có hàm khác cập nhật)
extern uint8_t relay_state; // 0 = đóng, 1 = mở

void lcd_update_status(void)
{
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_SendString("Nhap mat khau");   // Dòng 1 cố định

    LCD_SetCursor(1, 0);
    if (relay_state) {
        LCD_SendString("Trang thai: Mo ");
    } else {
        LCD_SendString("Trang thai: Dong");
    }
}
