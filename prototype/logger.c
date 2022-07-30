// logger.c
// No copyright intended.
//
// Code for a logging function.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <logger.h>

const char* const type_chars[] = {
    "[Misc]\t",
    "[Parse]\t",
    "[Exec]\t",
    "[Misc]\t",
    "[Misc]\t",
};

const char* const weight_chars[] = {
    "[I] ",
    "[I] ",
    "[W] ",
    "[E] ",
    "[A] ",
};

int type_weights[20] = {0};

void clogg(const int type, const int weight, const char* format, ...){
    if(type_weights[type] > weight) return;
    printf(type_chars[type]);
    printf(weight_chars[weight]);
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    printf("\n");
    if(weight == CLOG_ABORT) abort();
}

void clogg_set(const int type, const int weight){
    type_weights[type] = weight;
}
