// intrpr_interpreter.h
// No copyright intended.
//
// This is like the public interface for the interpeter/compilser.

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <intrpr_structures.h>
void interpreter_module_clear(struct intepreter_module* mod);
void link(struct intepreter_module* mod);
void interpreter_garbage_collect();
void interpret(struct intepreter_module* mod, unsigned short module_index, unsigned short bytecode_offset);

unsigned short interpreter_module_add(char* const name, char* const filename);
void interpreter_module_load_all();
void interpreter_interpret_from_console();

void interpeter_init();

unsigned short interpreter_compile_action(const char* code);
unsigned short interpreter_interpret_action(unsigned short action);

void decompile(struct intepreter_module* mod);

//extern int INTERPRETER_STATE;

#endif // INTERPRETER_H
