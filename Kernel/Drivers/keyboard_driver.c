// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/keyboard_driver.h"
#include "../include/keyboard_buffer.h"
#include "../include/interrupts.h"

#include "include/video_driver.h"
#include "libasm.h"
#include "../include/process_manager.h"
#include "../include/scheduler.h"
#include "../include/pipe_manager.h"

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define CTRL 0x1D
#define CTRL_RLS 0x9D
#define EXTENDED_KEY 0xE0

static uint8_t keyMapRow = 0;
static uint8_t control_pressed = 0;

static uint8_t scancodeToAscii[] = {

    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static uint8_t scancodeShiftToAscii[] = {

    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static uint8_t *keyMap[] = {scancodeToAscii, scancodeShiftToAscii};

void keyboard_handler()
{
  uint16_t code = getKey();

  if (code == CTRL)
  {
    control_pressed = 1;
    return;
  }
  else if (code == CTRL_RLS)
  {
    control_pressed = 0;
    return;
  }
  else if (code == EXTENDED_KEY)
  {
    code = getKey();
    if (code == CTRL)
    {
      control_pressed = 1;
      return;
    }
    else if (code == CTRL_RLS)
    {
      control_pressed = 0;
      return;
    }
  }

  if (code < 0x80)
  { // Key pressed
    char *buff = getBufferAddress();
    int buff_pos = getBufferPosition();
    if (code == LEFT_SHIFT || code == RIGHT_SHIFT)
    {
      keyMapRow = 1;
    }
    else if (control_pressed)
    {
      if (keyMap[keyMapRow][code] == 'c' || keyMap[keyMapRow][code] == 'C')
      {
          kill_foreground_process();
          return;
      }
      else if(keyMap[keyMapRow][code] == 'd' || keyMap[keyMapRow][code] == 'D')
      {
          send_eof_signal();
          return;
      }
    }
    else if (keyMap[keyMapRow][code] != 0)
    {
      buff[buff_pos] = keyMap[keyMapRow][code];
      incBufferLen(1);
      setPos(buff_pos);
    }
  }
  else
  { // Key released
    code -= 0x80;
    if (code == LEFT_SHIFT || code == RIGHT_SHIFT)
    {
      keyMapRow &= 0xFE;
    }
  }

  wake_up_shell();

  return;
}
