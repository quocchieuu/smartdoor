/* DS1307 library for STM32 HAL
 * Fixed version for standard DS1307 IC
 * Author: adapted for your project
 */

#include "ds1307_for_stm32_hal.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

I2C_HandleTypeDef *_ds1307_ui2c;

/* ----------------- Helpers ----------------- */
static uint8_t DS1307_DecodeBCD(uint8_t bin) {
    return (((bin & 0xF0) >> 4) * 10) + (bin & 0x0F);
}

static uint8_t DS1307_EncodeBCD(uint8_t dec) {
    return (uint8_t)((dec % 10) | ((dec / 10) << 4));
}

/* ----------------- Init ----------------- */
void DS1307_Init(I2C_HandleTypeDef *hi2c) {
    _ds1307_ui2c = hi2c;
    DS1307_SetClockHalt(0);  // start clock
}

/* ----------------- Low-level read/write ----------------- */
void DS1307_SetRegByte(uint8_t regAddr, uint8_t val) {
    uint8_t bytes[2] = { regAddr, val };
    HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, bytes, 2, DS1307_TIMEOUT);
}

uint8_t DS1307_GetRegByte(uint8_t regAddr) {
    uint8_t val;
    HAL_I2C_Master_Transmit(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &regAddr, 1, DS1307_TIMEOUT);
    HAL_I2C_Master_Receive(_ds1307_ui2c, DS1307_I2C_ADDR << 1, &val, 1, DS1307_TIMEOUT);
    return val;
}

/* ----------------- Clock Halt ----------------- */
void DS1307_SetClockHalt(uint8_t halt) {
    uint8_t sec = DS1307_GetRegByte(DS1307_REG_SECOND);
    if (halt)
        sec |= 0x80;
    else
        sec &= ~0x80;
    DS1307_SetRegByte(DS1307_REG_SECOND, sec);
}

uint8_t DS1307_GetClockHalt(void) {
    return (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x80) >> 7;
}

/* ----------------- Get functions ----------------- */
uint8_t DS1307_GetSecond(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7F);
}

uint8_t DS1307_GetMinute(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MINUTE));
}

uint8_t DS1307_GetHour(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_HOUR) & 0x3F);
}

uint8_t DS1307_GetDayOfWeek(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DOW));
}

uint8_t DS1307_GetDate(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DATE));
}

uint8_t DS1307_GetMonth(void) {
    return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MONTH));
}

uint16_t DS1307_GetYear(void) {
    return 2000 + DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_YEAR));
}

/* ----------------- Set functions ----------------- */
void DS1307_SetSecond(uint8_t second) {
    uint8_t ch = DS1307_GetClockHalt() ? 0x80 : 0x00;
    uint8_t sec_bcd = DS1307_EncodeBCD(second & 0x7F);
    sec_bcd |= ch;
    DS1307_SetRegByte(DS1307_REG_SECOND, sec_bcd);
}

void DS1307_SetMinute(uint8_t minute) {
    DS1307_SetRegByte(DS1307_REG_MINUTE, DS1307_EncodeBCD(minute));
}

void DS1307_SetHour(uint8_t hour_24mode) {
    DS1307_SetRegByte(DS1307_REG_HOUR, DS1307_EncodeBCD(hour_24mode & 0x3F));
}

void DS1307_SetDayOfWeek(uint8_t dow) {
    DS1307_SetRegByte(DS1307_REG_DOW, DS1307_EncodeBCD(dow));
}

void DS1307_SetDate(uint8_t date) {
    DS1307_SetRegByte(DS1307_REG_DATE, DS1307_EncodeBCD(date));
}

void DS1307_SetMonth(uint8_t month) {
    DS1307_SetRegByte(DS1307_REG_MONTH, DS1307_EncodeBCD(month));
}

void DS1307_SetYear(uint16_t year) {
    if (year < 2000) year = 2000;
    if (year > 2099) year = 2099;
    uint8_t y = (uint8_t)(year - 2000);
    DS1307_SetRegByte(DS1307_REG_YEAR, DS1307_EncodeBCD(y));
}

/* ----------------- Example: set datetime ----------------- */
void RTC_SetDateTime(void) {
    //DS1307_SetSecond(0);
    DS1307_SetMinute(19);
    DS1307_SetHour(14);
    DS1307_SetDate(22);
    DS1307_SetMonth(9);
    DS1307_SetYear(2025);
    //DS1307_SetDayOfWeek(2); // Monday=1, Sunday=0 tùy bạn
}

#ifdef __cplusplus
}
#endif
