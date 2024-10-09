#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#define NUM_KEYS 128


typedef struct {
    unsigned char make;
    unsigned char break_code;
} KeyCode;

extern KeyCode ScanCodes[NUM_KEYS];

void keyboardHandler();
extern int get_key();
extern int getShiftVal();
extern void setShift(int i);

#endif
