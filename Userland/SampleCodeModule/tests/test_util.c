// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include "include/test_util.h"
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"
// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t get_uint()
{
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t get_uniform(uint32_t max)
{
  uint32_t u = get_uint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory.
uint8_t memcheck(void *start, uint8_t value, uint32_t size)
{
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
    {
      return 0;
    }

  return 1;
}

// Parameters
int64_t satoi(char *str)
{
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-')
  {
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i)
  {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}

// // Dummies
void bussy_wait(uint64_t n)
{
  uint64_t i;
  for (i = 0; i < n; i++)
    ;
}

void endless_loop()
{
  while (1)
    ;
}

void endless_loop_print(uint64_t argc, char **argv)
{
  uint64_t wait = satoi(argv[1]);
  int64_t pid = call_get_current_pid();

  while (1)
  {
    print(0xFFFFFF, "%d ", pid);
    bussy_wait(wait);
  }
}