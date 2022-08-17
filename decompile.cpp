
#include "ligma.h"

namespace ligma {
    void Bytecode::decompile () {
        printf("decompile. bytecode length: %d\n", bytecode_last);
        for(int i = 0; i < bytecode_last; i++){
            printf("%03d: %s ", i, bytecode_instruction_str[
                    bytecode[
                            i]]);

            switch(bytecode[i]){
                /*case EXECUTEWORD:
                    i++;
                    printf("%d", *((unsigned short*)(bytecode + i)));
                    i++;
                    break;*/
                case PUSHWORD:
                    i++;
                    printf("%d", *((unsigned short*)(bytecode + i)));
                    i++;
                    break;
                case JUMP:
                    i++;
                    printf("%d", *((unsigned short*)(bytecode + i)));
                    i++;
                    break;
                case JUMPIFNOT:
                    i++;
                    printf("%d", *((unsigned short*)(bytecode + i)));
                    i++;
                    break;
                case VECTORIZE:
                    i++;
                    printf("%d", *((unsigned short*)(bytecode + i)));
                    i++;
                    break;
                case PUSHFLOAT32:
                    i++;
                    printf("%d [%f|%d]", *((unsigned short*)(bytecode + i)),
                           *((float*)(literals + *((unsigned short*)(bytecode + i)))),
                           *((unsigned short*)(bytecode + i + 2)));
                    i++;i++;i++;
                    break;
                case PUSHINT64:
                    i++;
                    printf("%d [%d|%d]", *((unsigned short*)(bytecode + i)),
                           *((int64_t*)(literals + *((unsigned short*)(bytecode + i)))),
                           *((unsigned short*)(bytecode + i + 2)));
                    i++;i++;i++;
                    break;
                case PUSHUINT64:
                    i++;
                    printf("%d [%d|%d]", *((unsigned short*)(bytecode + i)),
                           *((uint64_t*)(literals + *((unsigned short*)(bytecode + i)))),
                           *((unsigned short*)(bytecode + i + 2)));
                    i++;i++;i++;
                    break;
                case PUSH_STRING:
                    i++;
                    printf("%d [%s]", *((unsigned short*)(bytecode + i)),
                           literals + *((unsigned short*)(bytecode + i)));
                    i++;i++;i++;
                    break;
                case PUSH_LAMBDA:
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



}