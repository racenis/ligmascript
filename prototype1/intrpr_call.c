#include <stdio.h>

void hellowow(){
    printf("Hewwwoow uwu ;33 \n");
}

void (*call_fs[])(void) = {
    hellowow,
    hellowow,
    hellowow
};

const unsigned long max_call_fs = sizeof(call_fs) / sizeof(void*);
