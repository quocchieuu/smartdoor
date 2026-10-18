#ifndef __OLED_H
#define __OLED_H

typedef enum {
    OLED_MAIN,          // màn hình chính
    OLED_ENTER_PASS,    // nhập mật khẩu
    OLED_FINGER_ENROLL, // enroll vân tay
    OLED_DELETE_FP,     // xóa vân tay
    OLED_MESSAGE        // thông báo tạm thời
} OLED_Mode_t;

extern OLED_Mode_t oled_mode;

#endif
