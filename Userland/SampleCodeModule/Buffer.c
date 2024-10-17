#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usr_sys_calls.h"

void clear_buffer(char *buffer)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = 0;
    }
}
