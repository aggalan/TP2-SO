
#ifndef keyboard_buffer_h
#define keyboard_buffer_h
#include <stdint.h>
#define BUFF_SIZE 4096
typedef struct keyboard_buffer *buff_ptr;

struct keyboard_buffer
{
    int buffer_pos;
    int buffer_len;
    char buffer[BUFF_SIZE];
};

int get_buffer_position();            // Returns the current position of the buffer
void set_pos(int newPosition);        // Set the buffer position to a new position
char *get_buffer_address();           // Get the buffer address
char get_char_at(int position);       // Return the char located at buffer[position]
void consume_buffer_at(int position); // Erases the char located at position
void clean_buffer();                  // cleans the buffer
int get_buffer_len();                 // Returns the current size of the buffer
void inc_buffer_len(int n);           // increments the current size of the buffer

#endif