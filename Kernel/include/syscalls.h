#ifndef SYSCALLS_H
#define SYSCALLS_H
#define STDIN 0
#define STDOUT 1
#define ERROUT 2

// writes in the file descriptor received the string, of length len in a color of HexColor
void sys_write(int descriptor, const char *str, int len, uint32_t hexColor);

// reads from the character buffer until reaching the amount of characters len or emptying the buffer and saves the chars at save
int sys_read(int descriptor, char *save, int len);

#endif // TPE_SYSCALLS_H
