// intrpr_control.c
// No copyright intended.
//
// This file contains random stuff for like stuff.

#include <intrpr_interpreter.h>
#include <intrpr_structures.h>
#include <intrpr_parser.h>
#include <logger.h>

#include <stdio.h>
#include <assert.h>

struct intepreter_module interpreter_modules[INTERPRETER_MAX_MODULES];
int interpreter_module_last = 0;

void interpreter_init(){
    // reserve the module id 0 for live interpreting mode
    interpreter_module_add("interpret", "[nofilename]");

    // reserve the module id 1 for entity actions
    interpreter_module_add("entityaction", "[nofilename]");

    printf("LIGMAScript Interpreter v0.0.69\n");
}

unsigned short interpreter_module_add(char* const name, char* const filename){
    struct intepreter_module* mod = &interpreter_modules[interpreter_module_last];
    mod->name = malloc(strlen(name) + 1);
    mod->filename = malloc(strlen(filename) + 1);
    strcpy(mod->name, name);
    strcpy(mod->filename, filename);
    interpreter_module_last++;
}

void interpreter_module_load_all(){
    static int loaded_before = 0;
    assert(!loaded_before);
    loaded_before = 1;

    char buffer[100];
    interpreter_module_clear(&interpreter_modules[0]);
    interpreter_module_clear(&interpreter_modules[1]);
    for (int i = 2; i < interpreter_module_last; i++){
        clogg(CLOG_PARSE, CLOG_INFO, "Loading module '%s'...", interpreter_modules[i].name);
        interpreter_module_clear(&interpreter_modules[i]);
        parser_mount_module(&interpreter_modules[i], i);

        FILE* input = fopen(interpreter_modules[i].filename, "r");

        if(!input){
            clogg(CLOG_PARSE, CLOG_ERROR, "File '%s' couldn't be opened!", interpreter_modules[i].filename);
        } else {
            clogg(CLOG_PARSE, CLOG_INFO, "File ok.");
        }

        clogg(CLOG_PARSE, CLOG_INFO, "Compiling bytecode...");
        while(fscanf(input, "%s", buffer) > 0){
            //printf("%s\n", buffer);
            parse_token(buffer);
        }

        //decompile(&interpreter_modules[i]);
        clogg(CLOG_PARSE, CLOG_INFO, "Linking words...");
        link(&interpreter_modules[i]);
        clogg(CLOG_PARSE, CLOG_INFO, "Linking done!");
        clogg(CLOG_PARSE, CLOG_INFO, "Displaying bytecode...");
        decompile(&interpreter_modules[i]);
        clogg(CLOG_PARSE, CLOG_INFO, "Executing...");
        interpret(&interpreter_modules[i], i, 0);

        fclose(input);
    }

}

unsigned short interpreter_compile_action(const char* code){
    char* buffer;
    struct intepreter_module* mod = &interpreter_modules[1];
    parser_mount_module(mod, 1);

    unsigned short bytecode = mod->bytecode_last;

    buffer = strtok(code, " ");
    while(buffer){
        parse_token(buffer);
        buffer = strtok(NULL, " ");
    }
    link(mod);
    while(mod->first_in){
        void* h = mod->first_in->next;
        free(mod->first_in);
        mod->first_in = h;
    }
    mod->first_in = NULL;
    mod->last_in = NULL;

    return bytecode;
}

unsigned short interpreter_interpret_action(unsigned short action){
    interpret(&interpreter_modules[1], 1, action);
    return 1;
}

void interpreter_interpret_from_console(){
    char buffer[100];
    struct intepreter_module* mod = &interpreter_modules[0];
    parser_mount_module(mod, 0);

    unsigned short bytecode;

    while(scanf("%s", buffer) > 0){
        if(strcmp(buffer, "exit") == 0) return;
        if(strcmp(buffer, "decompile") == 0){decompile(mod); continue;}
        if(strcmp(buffer, "collect") == 0){interpreter_garbage_collect(); continue;}

        printf("%s\n", buffer);

        if(PARSER_STATE == INTRPR_PARSE_NORMAL) bytecode = mod->bytecode_last;

        parse_token(buffer);

        if(PARSER_STATE == INTRPR_PARSE_NORMAL) {
            link(mod);

            //delete the list of linkable words
            while(mod->first_in){
                void* h = mod->first_in->next;
                free(mod->first_in);
                mod->first_in = h;
            }
            mod->first_in = NULL;
            mod->last_in = NULL;

            interpret(mod, 0, bytecode);
        }


    }

}
