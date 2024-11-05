// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "keyboard_buffer.h"
static struct keyboard_buffer buff = {0, 0, {0}};

static buff_ptr ptr = &buff;

void inc_buffer_len(int n)
{
    ptr->buffer_len += n;
}

int get_buffer_position()
{
    return ptr->buffer_pos;
}
void set_pos(int newPosition)
{
    ptr->buffer_pos = newPosition % BUFF_SIZE;
}
char *get_buffer_address()
{
    return ptr->buffer;
}
char get_char_at(int position)
{
    return ptr->buffer[position % BUFF_SIZE];
}
void consume_buffer_at(int pos)
{
    ptr->buffer[pos] = 0;
    inc_buffer_len(-1);
    set_pos(pos + 1);
}
void clean_buffer()
{
    char *bufferAux = ptr->buffer;
    for (int i = 0; i < ptr->buffer_len; i++)
    {
        bufferAux[i] = '\0';
    }
}
int get_buffer_len()
{
    return ptr->buffer_len;
}
