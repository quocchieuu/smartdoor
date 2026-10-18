#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h" // hoặc HAL phù hợp

#define UART_HANDLER huart1  // đổi theo UART bạn dùng
#define MAX_FINGERPRINTS 10

// Packet
typedef struct {
    uint16_t start_code;
    uint32_t address;
    uint8_t type;
    uint16_t length;
    uint8_t data[64];
    uint16_t checksum;
} F_Packet;

// Commands
#define FINGERPRINT_STARTCODE       0xEF01
#define FINGERPRINT_COMMANDPACKET   0x01

#define FINGERPRINT_GETIMAGE        0x01
#define FINGERPRINT_IMAGE2TZ        0x02
#define FINGERPRINT_REGMODEL        0x05
#define FINGERPRINT_STORE           0x06
#define FINGERPRINT_SEARCH          0x04
#define FINGERPRINT_DELETE          0x0C
#define FINGERPRINT_TEMPLATECOUNT   0x1D
#define FINGERPRINT_READSYSPARAM    0x0F
#define FINGERPRINT_EMPTY           0x0D

// ACK codes
#define FINGERPRINT_OK              0x00
#define FINGERPRINT_NOFINGER        0x02
#define FINGERPRINT_TIMEOUT         0xFF

// State machine
typedef enum {
    FP_IDLE = 0,
    FP_GRANTED,
    FP_DENIED,
    FP_WAIT_RESET
} FingerprintState_t;

// API
void init_fingerprint(void);
bool is_finger_on_sensor(void);
uint8_t save_fingerprint_loop(uint8_t finger_id);
uint8_t check_fingerprint(uint8_t *finger_id);
uint16_t get_template_number(void);
uint8_t delete_fingerprint(uint16_t id);
void show_fingerprint_count(void);
void check_fingerprint_display(void);

// Packet helpers
void setup_packet(uint8_t *data, uint8_t size);
void send(void);
void receive(void);

#endif
