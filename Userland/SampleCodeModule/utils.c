#include <stdio.h>
#include <stdarg.h>
#include <utils.h>
#include <userSyscalls.h>
#include <buffer.h>


void putCh(char c){
    call_sys_write(&c, 1, STDOUT);
}

char getCh(){
    char c;
    call_sys_read(&c, 1, STDIN);
    return c;
}

int nullOrSpace(char c){
    return (c == '\0' || c == ' ');
}

int nullOrNewline(char c){
    return (c == '\0' || c == '\n');
}

int strcmp(char * str1, char * str2){
	int i = 0;
	for (; str1[i] != '\0' && str2[i] != '\0'; i++){
		if (str1[i] > str2[i]){
			return 1;
		} else if (str1[i] < str2[i]){
			return -1;
		}
	}
	if (str1[i] == '\0' && str2[i] == '\0') {
        return 0;
    } else if (str1[i] == '\0') {
        return -1; 
    } else return 1;

}

char toUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}


int strcmpspace(char *str1, char *str2) {
    // Compare characters up to the first space in both strings
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0' && !nullOrSpace(str2[i])) {
        if (toUpper(str1[i]) != toUpper(str2[i])) {
            return toUpper(str1[i]) > toUpper(str2[i]) ? 1 : -1;
        }
        i++;
    }
    // If str2 contains space, compare up to that space
    if (str1[i] == '\0' && nullOrSpace(str2[i])) {
        return 0;
    } else if (nullOrSpace(str2[i])) {
        return -1;
    } else {
        return 1;
    }
}

void print( char * fmt, ...){
    va_list args;
    va_start(args, fmt);

    while(*fmt != '\0'){
        if(*fmt == '%'){
            fmt++;
            switch(*fmt){
                case 'd':{
                    int d = va_arg(args, int *) ;
                    char buff[BUFFERLIMIT];
                    intToStr(d, buff, 10);
                    call_sys_write(buff, strlen(buff), STDOUT);
                    break;
                }
                case 's':{
                    char * s = va_arg(args, char *);
                    call_sys_write(s, strlen(s), STDOUT);
                    break;
                }
                case 'c':{
                    char c = va_arg(args, int);
                    call_sys_write(&c, 1, STDOUT);
                    break;
                }
            }
        } else {
            call_sys_write(fmt, 1, STDOUT);
        }
        fmt++;
    }
    
    va_end(args);
    return;
}

void intToStr(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

int strlen(char * str){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}


int scan(char * fmt, ...){
    va_list args;
    va_start(args, fmt);
    int i = 0;

    while(*fmt != '\0'){
        if(*fmt == '%'){
            fmt++;
            switch(*fmt){
                case 'd':{
                    int * d = va_arg(args, int *);
                    i+= readInt(d);
                    break;
                }
                case 's':{
                    char * s = va_arg(args, char *);
                    i += readStr(s);
                    break;
                }
                case 'c':{
                    char * c = va_arg(args, char *);
                    * c = getCh();
                    i++;
                    break;
                }
            }
        }
        fmt++;
    }
    
    va_end(args);
    return i;
}

int readStr(char * buff){
    int i = 0;
    char c = getCh();
    while(!nullOrNewline(c)){
        buff[i++] = c;
        c = getCh();
    }
    buff[i] = '\0';
    return i;
}

int readInt(int * num){
    int val = 0;
    char c = getCh();
    int sign = 1;
    if(c == '-'){
        sign = -1;
        c = getCh();
    }
    while( c != 0 && (c >= '0' && c <= '9')){
        val = val * 10 + (c - '0');
        c = getCh();
    }
    *num = val * sign;
    return 1;
}
