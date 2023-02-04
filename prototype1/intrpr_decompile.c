// intrpr_decompile.c
// No copyright intended.
//
// Decompile function. Goes through a module's bytecode and prints it out.

#include <intrpr_interpreter.h>
#include <intrpr_instructions.h>

#include <stdio.h>

void decompile(struct intepreter_module* mod){
    unsigned char* bytecode = mod->bytecode;
    unsigned char* literals = mod->literals;

    printf("decompile. bytecode length: %d\n", mod->bytecode_last);
    for(int i = 0; i < mod->bytecode_last; i++){
        printf("%03d: %s ", i, instr_str[
               bytecode[
               i]]);

        switch(bytecode[i]){
            case INTRPR_EXECUTEWORD:
                i++;
                printf("%d", *((unsigned short*)(bytecode + i)));
                i++;
                break;
            case INTRPR_JUMP:
                i++;
                printf("%d", *((unsigned short*)(bytecode + i)));
                i++;
                break;
            case INTRPR_VECTORIZE:
                i++;
                printf("%d", *((unsigned short*)(bytecode + i)));
                i++;
                break;
            case INTRPR_PUSHFLOAT32:
                i++;
                printf("%d [%f]", *((unsigned short*)(bytecode + i)),
                       *((float*)(literals + *((unsigned short*)(bytecode + i)))));
                i++;
                break;
            case INTRPR_PUSH_STRING:
                i++;
                printf("%d [%s]", *((unsigned short*)(bytecode + i)),
                       literals + *((unsigned short*)(bytecode + i)));
                i++;
                break;
            case INTRPR_PUSH_LAMBDA:
                i++;
                printf("%d", *((unsigned short*)(bytecode + i)));
                i++;
                break;
            default:
                break;

        }

        printf("\n");
    }
    printf("decompile done\n");
}
