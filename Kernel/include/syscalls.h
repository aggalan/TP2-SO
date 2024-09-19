#ifndef _SYSCALLS_H
#define _SYSCALLS_H

void sys_write(char * buffer, int len, int fd);
void sys_read(char * buff, int len, int fds);
void setShift(int i);
int getShiftVal();


#define STDIN 0
#define STDOUT 1
#define STDERR 2

#endif
