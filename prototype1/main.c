// main.c
// No copyright intended.
//
// A simple example.

#include <stdio.h>

#include <intrpr_interpreter.h>


int main()
{
    // I don't remembere why this function is neccessary, but it is a good
    // idea to call it anyway.
    interpreter_init();

    // This will make the interpreter_module_load_all() function load
    // this file and then convert it to the bytecode and execute it.
    interpreter_module_add("testcode", "code.txt");

    // This loads all the files.
    interpreter_module_load_all();

    // This makes the program go into interpret mode where it will take input
    // from the standard input and then convert it to bytecode and then execute
    // it and then put the output into the standard output.
    interpreter_interpret_from_console();


    // I have no idea why this is here, but I am afraid to delete it.
    printf("Hello world!\n");

    // I used the strtok() so you can't put a literal into the
    // interpreter_compile_action() function, otherwise it will segfault.
    char AAA[100] = "420 69 + print" ;

    // This will convert the code to bytecode. And then return a handle, so
    // that it can be executed later.
    unsigned int act = interpreter_compile_action(AAA);

    // This executes the bytecode.
    interpreter_interpret_action(act);

    // This makes the program go away.
    return 0;
}
