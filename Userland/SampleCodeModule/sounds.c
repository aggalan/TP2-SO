#include "sounds.h"
#include "userSyscalls.h"


void collision(){

    call_sound(200, 261);
    call_sleepms(10);

    call_sound(200, 329);
    call_sleepms(10);

    call_sound(200, 392);
    call_sleepms(10);

    call_sound(200, 523);
    call_sleepms(10);
}


