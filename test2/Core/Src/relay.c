#include "relay.h"
#include "main.h"

uint8_t relay_state = 0; // mặc định cửa đóng
static uint32_t relay_on_time = 0;
static uint8_t relay_active = 0;
// ===== Cấu hình chân cảm biến cửa (NC) =====
#define DOOR_SENSOR_Pin   GPIO_PIN_12
#define DOOR_SENSOR_Port  GPIOB
#define DOOR_HOLD_TIME   3000   // 5 giây cho cửa mở
#define DOOR_MAX_OPEN    15000  // 60 giây nếu cửa không đóng thì cảnh báo

// Hàm kiểm tra cửa đóng/mở
static uint8_t is_door_closed(void) {
    // Cảm biến NC: đóng cửa = 0, mở cửa = 1
    return (HAL_GPIO_ReadPin(DOOR_SENSOR_Port, DOOR_SENSOR_Pin) == GPIO_PIN_RESET);
}


void relay_init(void)
{
    relay_off(); // khởi tạo relay ở trạng thái OFF
}

void relay_on(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
           osDelay(50);
           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
           osDelay(50);
           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
                   osDelay(50);
                   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
                   osDelay(50);
    relay_state = 1;
    relay_on_time = HAL_GetTick();  // lưu thời điểm bật
        relay_active = 1;
}

void relay_off(void)
{
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    relay_state = 0;
    relay_active = 0; // reset lại
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); // tắt LED cảnh báo
}

// Cập nhật relay theo state
void relay_update(uint8_t state)
{
    if (state)
        relay_on();
    else
        relay_off();
}

// Kiểm tra timeout + cảm biến từ
void CheckRelayTimeout(void)
{
    if (relay_active) {
        uint32_t elapsed = HAL_GetTick() - relay_on_time;

        // Nếu cửa đã đóng VÀ đã qua thời gian mở cửa tối thiểu (5s)
        if (is_door_closed() && elapsed >= DOOR_HOLD_TIME) {
            relay_off();
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); // tắt LED cảnh báo
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        }
        else {
            // Nếu cửa vẫn mở quá 60 giây => bật LED cảnh báo
            if (elapsed >= DOOR_MAX_OPEN) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); // LED cảnh báo
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); // LED đảo trạng thái
                osDelay(300);
            }
        }
    }
}



