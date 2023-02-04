// logger.h
// No copyright intended.
//
// Header for the logging function.

#ifndef CLOGGER_H
#define CLOGGER_H


// clogging types
#define CLOG_MISC 0
#define CLOG_PARSE 1
#define CLOG_INTERPRET 2

// clogging weights
#define CLOG_FULL 0
#define CLOG_INFO 1
#define CLOG_WARN 2
#define CLOG_ERROR 3
#define CLOG_ABORT 4

void clogg(const int type, const int weight, const char* format, ...);

void clogg_set(const int type, const int weight);

#endif
