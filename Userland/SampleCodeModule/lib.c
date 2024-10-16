#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include <stdarg.h>
#define TO_UPPER(c) (((c) >= 'a' && (c) <= 'z') ? ((c) - ('a' - 'A')) : (c))


int getC(char * c){
    return call_sys_read(STDIN,c, 1);
}
int getBuffer(char * buf){
    return call_sys_read(STDIN,buf, 100);
}
void putC(char c,uint32_t hexColor){
    call_sys_write( STDOUT,&c, 1,hexColor);
}

void putInt(int num,uint32_t hexColor){ 
        if (num < 0) {
            putC('-',hexColor);
            num = -num;
        }

        int divisor = 1;
        while (num / divisor >= 10) {
            divisor *= 10;
        }

        while (divisor > 0) {
            int digit = num / divisor;
            putC('0' + digit,hexColor);
            num %= divisor;
            divisor /= 10;
        }
}
void putString(const char * s,uint32_t hexColor){
    call_sys_write( STDOUT,s, strlen(s),hexColor);
}


void print(uint32_t hexColor,const char * str, ...){
    va_list args;
    va_start(args, str);


    while(*str != '\0'){
        if(*str == '%'){
            str++;

            switch(*str){
                case 'c': {
                    char * c = va_arg(args, char*);
                    putC(*c,hexColor);
                    break;
                }
                case 'd': {
                    int * d = va_arg(args, int*);
                    putInt(*d,hexColor);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    putString(s,hexColor);
                    break;
                }
            }
        } else {
            putC(*str,hexColor);
        }
        str++;
    }

    va_end(args);
    return;
}


int strlen(const char *str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
int strcmp( char *str1,  char *str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
char * cutString(char * str) {
    for(int i=0;str[i]!='\0';i++){
        if(str[i]==' '){
            str[i]='\0';
            return str;
        }
    }
    return str;
}

int pow(int base, int pow){
    int toR= 1;
    for(int i=0; i< pow;i++){
        toR*= base;
    }
    return toR;
}
int strToInt(char * str){
    int n = strlen(str);
    int rta=0;
    for(int i=0; i<n; i++){
       rta += (str[i] - '0') * pow(10,n-i-1);
    }
    return rta;
}


// Function to convert a string to uppercase
void strToUpper(char *str) {
    while (*str) {
        *str = TO_UPPER(*str);
        str++;
    }
}