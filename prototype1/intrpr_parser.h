// intrpr_parser.h
// No copyright intended.
//
// Uhh IDK what this dose.

#ifndef COMPILER_H
#define COMPILER_H

#define INTRPR_PARSE_NORMAL 0
#define INTRPR_PARSE_BLOCKSTARTED 1
#define INTRPR_PARSE_LAMBDA 2
#define INTRPR_PARSE_LIST 3
#define INTRPR_PARSE_STRING 4
#define INTRPR_PARSE_DEFVARNAME 6

extern int PARSER_STATE;

// you need to call this, so the parse_token() knows in which module to put
// in the resulting bytecode
void parser_mount_module(struct intepreter_module* mod, unsigned short index);

// converts tokens into bytecode
void parse_token(const char* buffer);

#endif // COMPILER_H
