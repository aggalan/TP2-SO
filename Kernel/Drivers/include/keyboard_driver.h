
#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

// saves received characters from the keyboard in a buffer, if character is ':' takes a screenshot of the registers
void keyboard_handler();
void keyboard_init();
int get_keyboard_sem();

#endif