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
#include "../include/lib.h"

#define EOF -1

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define CTRL 0x1D
#define CTRL_RLS 0x9D
#define EXTENDED_KEY 0xE0
#define ESC 0x01
#define LEFT_ALT 0x38
#define RIGHT_ALT 0xB8
#define DELETE 0x53
#define CAPS_LOCK 0x3A

static uint8_t key_map_row = 0;
static uint8_t control_pressed = 0;
static uint8_t caps_lock_pressed = 0;

static uint8_t scancode_to_ascii[] = {

    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static uint8_t scancode_shift_to_ascii[] = {

    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static uint8_t *key_map[] = {scancode_to_ascii, scancode_shift_to_ascii};

void keyboard_handler()
{
  uint16_t code = get_key();

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
    code = get_key();
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
    char *buff = get_buffer_address();
    int buff_pos = get_buffer_position();
    if (code == LEFT_SHIFT || code == RIGHT_SHIFT)
    {
      key_map_row = 1;
      return;
    }
    else if (code == ESC || code == LEFT_ALT || code == RIGHT_ALT || code == DELETE)
    {
      return;
    }
    else if (code == CAPS_LOCK)
    {
      caps_lock_pressed = !caps_lock_pressed;
      return;
    }
    else if (control_pressed)
    {
      if (key_map[key_map_row][code] == 'c' || key_map[key_map_row][code] == 'C')
      {
        kill_foreground_process();
        print_kernel(WHITE, "\n");
        return;
      }
      else if (key_map[key_map_row][code] == 'd' || key_map[key_map_row][code] == 'D')
      {
        buff[buff_pos] = EOF;
        inc_buffer_len(1);
      }
    }
    else if (key_map[key_map_row][code] != 0)
    {
      if (caps_lock_pressed)
      {
        buff[buff_pos] = key_map[1][code]; 
      }
      else
      {
        buff[buff_pos] = key_map[0][code];
      }
      inc_buffer_len(1);
      set_pos(buff_pos);
    }

    wake_up_shell();
  }
  else
  { // Key released
    code -= 0x80;
    if (code == LEFT_SHIFT || code == RIGHT_SHIFT)
    {
      key_map_row &= 0xFE;
    }
    else if (code == ESC)
    {
 
    }
    else if (key_map[key_map_row][code] == 0)
    {
      return;
    }
  }

  return;
}