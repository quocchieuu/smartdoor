#include "fingerprint.h"
#include "cmsis_os.h"
#include "relay.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h>
#include <stdio.h>
#include "lcd.h"    // để dùng ResetLCD()

extern UART_HandleTypeDef UART_HANDLER;
extern volatile uint8_t finger_busy;
static FingerprintState_t fp_state = FP_IDLE;
static uint32_t fp_timestamp = 0;
static uint8_t fingerprint_id;
uint16_t capacity = 64;  // Tổng số template trên module

F_Packet fpacket = {
    .start_code = FINGERPRINT_STARTCODE,
    .address    = 0xFFFFFFFF,
    .type       = FINGERPRINT_COMMANDPACKET
};

F_Packet rpacket;

/* ------------------- Packet ------------------- */
void setup_packet(uint8_t *data, uint8_t size) {
    if(size > 64) return;
    fpacket.length = size;
    uint16_t sum = fpacket.type + ((size+2)>>8) + ((size+2)&0xFF);
    for(int i=0;i<size;i++){
        fpacket.data[i] = data[i];
        sum += data[i];
    }
    fpacket.checksum = sum;
}

void send(void){
    uint8_t value;
    for(int i=0;i<2;i++){
        value = (fpacket.start_code >> (8*(1-i))) & 0xFF;
        HAL_UART_Transmit(&UART_HANDLER,&value,1,100);
    }
    for(int i=0;i<4;i++){
        value = (fpacket.address >> (8*(3-i))) & 0xFF;
        HAL_UART_Transmit(&UART_HANDLER,&value,1,100);
    }
    value = fpacket.type;
    HAL_UART_Transmit(&UART_HANDLER,&value,1,100);

    uint16_t len = fpacket.length + 2;
    for(int i=0;i<2;i++){
        value = (len >> (8*(1-i))) &0xFF;
        HAL_UART_Transmit(&UART_HANDLER,&value,1,100);
    }
    for(int i=0;i<fpacket.length;i++){
        value = fpacket.data[i];
        HAL_UART_Transmit(&UART_HANDLER,&value,1,100);
    }
    for(int i=0;i<2;i++){
        value = (fpacket.checksum >> (8*(1-i))) &0xFF;
        HAL_UART_Transmit(&UART_HANDLER,&value,1,100);
    }
}

void receive(void){
    uint8_t header[9];
    if(HAL_UART_Receive(&UART_HANDLER,header,9,2000)!=HAL_OK){
        rpacket.data[0]=0xFF;
        return;
    }
    rpacket.start_code = (header[0]<<8)|header[1];
    rpacket.address = (header[2]<<24)|(header[3]<<16)|(header[4]<<8)|header[5];
    rpacket.type = header[6];
    rpacket.length = (header[7]<<8)|header[8];

    uint16_t data_len = rpacket.length;
    if(data_len>=2 && data_len<=sizeof(rpacket.data)+2){
        uint8_t buf[128];
        if(HAL_UART_Receive(&UART_HANDLER,buf,data_len,2000)==HAL_OK){
            for(int i=0;i<data_len-2;i++) rpacket.data[i]=buf[i];
            rpacket.checksum = (buf[data_len-2]<<8)|buf[data_len-1];
        }else{
            rpacket.data[0]=0xFE;
        }
    }
}

/* -------------------- Fingerprint init -------------------- */
void init_fingerprint(void){
    uint8_t cmd = FINGERPRINT_READSYSPARAM;
    setup_packet(&cmd,1);
    send();
    receive();
}

/* -------------------- Finger check -------------------- */
bool is_finger_on_sensor(void){
    uint8_t cmd = FINGERPRINT_GETIMAGE;
    setup_packet(&cmd,1);
    send();
    receive();
    return (rpacket.data[0]==FINGERPRINT_NOFINGER?false:true);
}

/* -------------------- Save fingerprint -------------------- */
uint8_t save_fingerprint_loop(uint8_t finger_id) {
    uint8_t data[5];
    uint32_t start;

    // ------------------- Place Finger 1 -------------------
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Place finger 1", Font_11x18, White);
    ssd1306_UpdateScreen();

    start = HAL_GetTick();
    while (!is_finger_on_sensor()) {
        if (HAL_GetTick() - start > 15000) {
            return FINGERPRINT_TIMEOUT;
        }
        osDelay(50);
    }

    data[0] = FINGERPRINT_GETIMAGE;
    setup_packet(data, 1); send(); receive();
    if (rpacket.data[0] != FINGERPRINT_OK) return rpacket.data[0];

    data[0] = FINGERPRINT_IMAGE2TZ; data[1] = 1;
    setup_packet(data, 2); send(); receive();
    if (rpacket.data[0] != FINGERPRINT_OK) return rpacket.data[0];

    // remove finger
    ssd1306_Fill(Black);
    ssd1306_WriteString("Remove finger", Font_11x18, White);
    ssd1306_UpdateScreen();
    osDelay(2000);
    while (is_finger_on_sensor()) osDelay(100);

    // ------------------- Place Finger 2 -------------------
    ssd1306_Fill(Black);
    ssd1306_WriteString("Place finger 2", Font_11x18, White);
    ssd1306_UpdateScreen();

    start = HAL_GetTick();
    while (!is_finger_on_sensor()) {
        if (HAL_GetTick() - start > 15000) {
            return FINGERPRINT_TIMEOUT;
        }
        osDelay(50);
    }

    data[0] = FINGERPRINT_GETIMAGE;
    setup_packet(data, 1); send(); receive();
    if (rpacket.data[0] != FINGERPRINT_OK) return rpacket.data[0];

    data[0] = FINGERPRINT_IMAGE2TZ; data[1] = 2;
    setup_packet(data, 2); send(); receive();
    if (rpacket.data[0] != FINGERPRINT_OK) return rpacket.data[0];

    // REGMODEL
    data[0] = FINGERPRINT_REGMODEL;
    setup_packet(data, 1); send(); receive();
    if (rpacket.data[0] != FINGERPRINT_OK) return rpacket.data[0];

    // STORE template
    data[0] = FINGERPRINT_STORE; data[1] = 1;
    data[2] = (finger_id >> 8) & 0xFF;
    data[3] = finger_id & 0xFF;
    setup_packet(data, 4); send(); receive();

    return rpacket.data[0]; // 0x00 = OK
}

/* -------------------- Check fingerprint -------------------- */
uint8_t check_fingerprint(uint8_t *finger_id){
    uint8_t data[6];

    data[0]=FINGERPRINT_GETIMAGE; setup_packet(data,1); send(); receive();
    if(rpacket.data[0]!=FINGERPRINT_OK) return -3;

    data[0]=FINGERPRINT_IMAGE2TZ; data[1]=2; setup_packet(data,2); send(); receive();
    if(rpacket.data[0]!=FINGERPRINT_OK) return -3;

    data[0]=FINGERPRINT_SEARCH; data[1]=2; data[2]=0x00; data[3]=0x00;
    data[4]=(capacity>>8)&0xFF; data[5]=capacity&0xFF;
    setup_packet(data,6); send(); receive();

    if(rpacket.data[0]==0x09) return -1; // Not found
    else if(rpacket.data[0]!=FINGERPRINT_OK) return -2;
    else{
        *finger_id = (rpacket.data[1]<<8)|rpacket.data[2];
        return 0;
    }
}

/* -------------------- Template count -------------------- */
uint16_t get_template_number(void){
    uint8_t cmd=FINGERPRINT_TEMPLATECOUNT;
    setup_packet(&cmd,1); send(); receive();
    if(rpacket.data[0]==FINGERPRINT_OK)
        return ((rpacket.data[1]<<8)|rpacket.data[2]);
    return 0;
}

void show_fingerprint_count(void) {
    char msg[32];
    uint8_t count = get_template_number();
    sprintf(msg, "So luong: %d", count);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(msg, Font_11x18, White);
    ssd1306_UpdateScreen();

    osDelay(2000);
    ResetLCD();
}

/* -------------------- Delete fingerprint -------------------- */
uint8_t delete_fingerprint(uint16_t id) {
    if (id < 1 || id > MAX_FINGERPRINTS) {
        return 0xFF; // invalid
    }
    uint8_t data[5];
    data[0] = FINGERPRINT_DELETE;
    data[1] = (id >> 8) & 0xFF;
    data[2] = id & 0xFF;
    data[3] = 0x00;
    data[4] = 0x01;

    setup_packet(data, 5); send(); receive();
    return rpacket.data[0];
}

/* -------------------- State machine check -------------------- */
void check_fingerprint_display(void)
{
    uint8_t result;

    switch (fp_state)
    {
    case FP_IDLE:
        if (!finger_busy) {
            if (is_finger_on_sensor()) {
                osDelay(200);
                if (is_finger_on_sensor()) {
                    ssd1306_Fill(Black);
                    ssd1306_WriteString("Scanning...", Font_11x18, White);
                    ssd1306_UpdateScreen();

                    result = check_fingerprint(&fingerprint_id);

                    if (result == 0) {
                        ssd1306_Fill(Black);
                        ssd1306_WriteString("ACCESS GRANTED", Font_11x18, White);
                        ssd1306_UpdateScreen();

                        relay_on();
                        osDelay(2000);
                        ResetLCD();
                    } else {
                        ssd1306_Fill(Black);
                        ssd1306_WriteString("WRONG ID!", Font_11x18, White);
                        ssd1306_UpdateScreen();

                        osDelay(2000);
                        ResetLCD();
                    }
                }
            }
        }
        break;
    }
}
