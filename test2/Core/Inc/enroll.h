#ifndef ENROLL_H
#define ENROLL_H

typedef enum {
    ENROLL_IDLE = 0,
    ENROLL_PLACE1,
    ENROLL_IMAGE1,
    ENROLL_REMOVE,
    ENROLL_PLACE2,
    ENROLL_IMAGE2,
    ENROLL_REGMODEL,
    ENROLL_STORE,
    ENROLL_DONE
} EnrollState_t;

// Chỉ khai báo extern
extern EnrollState_t enroll_state;
extern int enroll_id;
extern uint8_t enroll_result;

#endif
