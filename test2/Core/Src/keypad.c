#include "keypad.h"
#include "liquidcrystal_i2c.h"
#define PASSWORD_LENGTH 4
#include "fingerprint.h"
#include "enroll.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "main.h"
#include "oled.h"
extern OLED_Mode_t oled_mode;
#define MAX_FINGERPRINTS 10  // Giới hạn tối đa 10 vân tay
char entered_password[PASSWORD_LENGTH + 1];
char saved_password[PASSWORD_LENGTH + 1] = "1869";
int password_index = 0;
int change_mode = 0;
// ------------------- Các trạng thái -------------------
// 0: nhập mật khẩu mở cửa
// 1: nhập pass cũ để đổi mật khẩu
// 2: nhập pass mới
// 3: nhập pass để thêm vân tay
// 4: nhập pass để xóa vân tay
// 5: nhập ID vân tay để xóa
// Toàn cục
extern volatile uint8_t finger_busy;
uint32_t relay_on_time = 0;   // thời điểm bật relay
uint8_t relay_active = 0;     // cờ trạng thái relay

// Ma trận phím (4x4)
const char keymap[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

GPIO_TypeDef* rowPorts[4] = {GPIOA, GPIOA, GPIOA, GPIOA};
uint16_t rowPins[4] = {Row1_Pin,Row2_Pin,Row3_Pin,Row4_Pin};

GPIO_TypeDef* colPorts[4] = {GPIOA, GPIOA, GPIOA,GPIOB};
uint16_t colPins[4] = {Col1_Pin,Col2_Pin,Col3_Pin,Col4_Pin};

// ------------------- RESET LCD -------------------
void ResetLCD(void) {
	oled_mode = OLED_MAIN;   // quay về màn hình chính
    password_index = 0;
    change_mode = 0;
}

void ResetLCD_Only(void) {
    password_index = 0;
    // Không reset change_mode
}

// ------------------- KHỞI TẠO -------------------
void keypad_init(void) {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(rowPorts[i], rowPins[i], GPIO_PIN_SET);
    }
}

// ------------------- Hiển thị tin nhắn -------------------
void display_msg(const char* msg) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(msg, Font_16x15, White);
    ssd1306_UpdateScreen();
}

// ------------------- QUÉT PHÍM -------------------
// Hàm quét keypad
char keypad_scan(void) {
    static char lastKey = 0;  // lưu phím lần trước
    char key = 0;

    for (int row = 0; row < 4; row++) {
        // Kéo dòng hiện tại xuống LOW
        HAL_GPIO_WritePin(rowPorts[row], rowPins[row], GPIO_PIN_RESET);

        for (int col = 0; col < 4; col++) {
            if (HAL_GPIO_ReadPin(colPorts[col], colPins[col]) == GPIO_PIN_RESET) {
                osDelay(5); // debounce nhanh

                if (HAL_GPIO_ReadPin(colPorts[col], colPins[col]) == GPIO_PIN_RESET) {
                    key = keymap[row][col];
                }
            }
        }

        // Trả lại mức HIGH
        HAL_GPIO_WritePin(rowPorts[row], rowPins[row], GPIO_PIN_SET);
    }

    // Chỉ trả về phím mới (tránh lặp khi giữ phím)
    if (key != 0 && key != lastKey) {
        lastKey = key;
        return key;
    }

    // Reset khi thả phím
    if (key == 0) {
        lastKey = 0;
    }

    return 0;
}

// ------------------- Xử lý mật khẩu mở cửa -------------------
void ProcessPassword(void) {
	//ssd1306_Fill(Black);
    if (strcmp(entered_password, saved_password) == 0) {
    	display_msg("Access Granted");
        relay_on();                     // mở relay
    } else {
    	display_msg("Wrong Password");
    }
    osDelay(2000);
    ResetLCD();
}



// ------------------- Xử lý đổi mật khẩu -------------------
void handle_password_change(void) {
    if (strcmp(entered_password, saved_password) == 0) {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("New Password:", Font_16x15, White);
        ssd1306_UpdateScreen();
        password_index = 0;
        change_mode = 2;
    } else {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Wrong Old Pass", Font_16x15, White);
        ssd1306_UpdateScreen();
        osDelay(2000);
        ResetLCD();
    }
}

void handle_new_password(void) {
    strcpy(saved_password, entered_password);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Password Changed", Font_16x15, White);
    ssd1306_UpdateScreen();
    osDelay(2000);
    ResetLCD();
}

// ------------------- Lưu mật khẩu mới -------------------
void handle_add_fingerprint(void) {
    if (strcmp(entered_password, saved_password) == 0) {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Enter Finger ID:", Font_16x15, White);
        ssd1306_UpdateScreen();
        password_index = 0;
        change_mode = 6;
        memset(entered_password, 0, sizeof(entered_password));
    } else {
        display_msg("Wrong Password!");
        ResetLCD();
    }
}


void handle_delete_fingerprint_auth(void) {
    if (strcmp(entered_password, saved_password) == 0) {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Enter Finger ID:", Font_16x15, White);
        ssd1306_UpdateScreen();
        password_index = 0;
        change_mode = 5;
    } else {
        display_msg("Wrong Password!");
        osDelay(2000);
        ResetLCD();
    }
}

void handle_delete_fingerprint_id(void) {
    int id = atoi(entered_password);
    uint8_t res = delete_fingerprint(id);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    if (res == FINGERPRINT_OK) {
        ssd1306_WriteString("Xoa thanh cong", Font_16x15, White);
    } else {
        ssd1306_WriteString("Xoa thanh cong", Font_16x15, White);
    }
    ssd1306_UpdateScreen();
    osDelay(2000);

    ResetLCD();
}





void InputPassword(char key)
{
    if (key >= '0' && key <= '9') {
    	if (password_index < PASSWORD_LENGTH) {
    	            entered_password[password_index++] = key;
    	        }
    	        oled_mode = OLED_ENTER_PASS;  // chỉ đổi trạng thái
    }
    else if (key == 'D') {
        ResetLCD();
    }
    else if (key == 'A') {
    	finger_busy = 1;
    	oled_mode = OLED_ENTER_PASS;
        password_index = 0;
        memset(entered_password, 0, sizeof(entered_password)); // 🔑 xoá pass cũ
        change_mode = 1;
        finger_busy = 0;
    }
    else if (key == 'B') {
    	finger_busy = 1;
    	oled_mode = OLED_ENTER_PASS;
        password_index = 0;
        memset(entered_password, 0, sizeof(entered_password)); // 🔑 xoá pass cũ
        change_mode = 3; // chuẩn bị nhập pass để thêm vân tay
    }
    else if (key == 'C') {
    	finger_busy = 1;
    	oled_mode = OLED_ENTER_PASS;
        password_index = 0;
        memset(entered_password, 0, sizeof(entered_password)); // 🔑 xoá pass cũ
        change_mode = 4;
        finger_busy = 0;
    }
    else if (key == '#') {
        entered_password[password_index] = '\0';

        switch(change_mode) {
            case 0: ProcessPassword(); break;
            case 1:
            	handle_password_change();
            	;break;
            case 2: handle_new_password(); break;
            case 3: // nhấn pass xong -> bắt đầu enroll
                            if(strcmp(entered_password, saved_password) == 0) {
                                finger_busy = 1;          // tạm dừng quét vân tay
                                display_msg("Enter Finger ID:");
                                LCD_SetCursor(1,0);
                                password_index = 0;
                                change_mode = 6;
                                memset(entered_password,0,sizeof(entered_password));
                            } else {
                            	display_msg("Wrong Password!");
                                osDelay(2000);
                                ResetLCD();
                            }
                            break;
            case 4: handle_delete_fingerprint_auth(); break;
            case 5: handle_delete_fingerprint_id(); return;
            case 6: // Nhập ID xong -> Enroll
            {
                int id = atoi(entered_password);

                if (id <= 0 || id > MAX_FINGERPRINTS) {
                	display_msg("Invalid ID!");
                    osDelay(2000);
                    ResetLCD_Only();
                    finger_busy = 0;   // bật lại quét fingerprint
                    break;
                }

                enroll_id = id;  // lưu ID
                display_msg("Enrolling Mode...");
                osDelay(500);
                uint8_t res = save_fingerprint_loop(enroll_id);
                if (res == FINGERPRINT_OK) {
                	display_msg("Enroll Success!");
                    // Đợi cảm biến ghi xong flash (quan trọng)
                        osDelay(2000);
                        show_fingerprint_count();


                } else {
                	display_msg("Enroll Fail");
                }

                osDelay(2000);
                ResetLCD_Only();
                password_index = 0;
                change_mode = 0;
                finger_busy = 0;   // bật lại quét fingerprint
                break;
            }

            				default:
                            break;

        }
        password_index = 0;
    }
    else if (key == '*') {
        if(password_index > 0) {
        	password_index--;
        	entered_password[password_index] = '\0';
        	display_msg(entered_password);
        }
    }
}
