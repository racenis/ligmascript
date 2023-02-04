// intrpr_interpreter.c
// No copyright intended.
//
// Code for interpreting the bytecode.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <intrpr_structures.h>
#include <intrpr_word_types.h>
#include <intrpr_instructions.h>
#include <intrpr_interpreter.h>
#include <intrpr_parser.h>
#include <logger.h>




// sttuff for the lists
struct interpreter_list_element{
    word_index word;
    struct interpreter_list_element* next;
};

struct interpreter_list{
    unsigned int refs;
    struct interpreter_list_element* first;
    struct interpreter_list_element* last;
};

// arrays for storing the handles and the words
struct interpreter_handle interpreter_handles[INTERPRETER_MAX_HANDLES];
struct interpreter_word interpreter_words[INTERPRETER_MAX_WORDS];
unsigned short interpreter_last_word = 0;

struct interpreter_list interpreter_lists[100];
int last_list = 0;

list_index interpreter_new_list(){
    short int l = last_list;
    last_list++;

    interpreter_lists[l].refs = 1;
    interpreter_lists[l].first = NULL;
    interpreter_lists[l].last = NULL;

    return l;
}

void interpreter_list_append(list_index list, word_index word){
    struct interpreter_list* l = &interpreter_lists[list];
    struct interpreter_list_element* e = malloc(sizeof(struct interpreter_list_element));

    e->next = NULL;
    e->word = word;

    interpreter_words[word].refs++;

    if(l->last){
        l->last->next = e;
        l->last = e;
    } else {
        l->first = e;
        l->last = e;
    }
}




word_index find_word(char* const word_name){
    int i = (hashong(word_name)) % INTERPRETER_MAX_HANDLES;
    while(interpreter_handles[i].name && strcmp(interpreter_handles[i].name, word_name) != 0) i++;
    if(interpreter_handles[i].name && strcmp(interpreter_handles[i].name, word_name) == 0)
        return interpreter_handles[i].word_offset;
    else
        return -1;
}

word_index insert_word_variable(int type, void* data_pointer){
    interpreter_words[interpreter_last_word].word_type = type;
    interpreter_words[interpreter_last_word].data_pointer = data_pointer;
    interpreter_words[interpreter_last_word].array_length = 1;
    interpreter_words[interpreter_last_word].flags = 0;
    interpreter_words[interpreter_last_word].refs = 0;
    unsigned short f = interpreter_last_word;
    interpreter_last_word++;
    if (interpreter_last_word >= INTERPRETER_MAX_WORDS) clogg(CLOG_INTERPRET, CLOG_ABORT, "Out of space for words!");
    return f;
}

word_index insert_word_bytecode(int type, unsigned short module, unsigned short offset){
    interpreter_words[interpreter_last_word].word_type = type;
    interpreter_words[interpreter_last_word].bytecode.module = module;
    interpreter_words[interpreter_last_word].bytecode.offset = offset;
    interpreter_words[interpreter_last_word].flags = 0;
    interpreter_words[interpreter_last_word].refs = 0;
    unsigned short f = interpreter_last_word;
    interpreter_last_word++;
    if (interpreter_last_word >= INTERPRETER_MAX_WORDS) clogg(CLOG_INTERPRET, CLOG_ABORT, "Out of space for words!");
    return f;
}

word_handle insert_handle(char* const word_name, unsigned short word){
    int i = (hashong(word_name)) % INTERPRETER_MAX_HANDLES;
    while(interpreter_handles[i].name) i++;
    interpreter_handles[i].name = word_name;
    interpreter_handles[i].word_offset = word;

    return i;
    //printf("\ninserting handle for %s with offset %d\n", word_name, word);
}

// releases the memory that the word points to
void interpreter_word_clean(struct interpreter_word* word){
    if(word->word_type == WORD_BYTECODE ||
       word->word_type == WORD_VARIABLE ||
       word->word_type == WORD_LIST){
        // bytecode shouldn't be tampered with,
        // unassigned variables have nothing to clean
        // lists have their own cleaning mechanism
        // TODO: decrement the list references here
        word->data_pointer = NULL;
        return;
    }

    if(word->flags & WORD_FLAG_NOFREE){
        // clear the NOFREE flag
        word->flags = word->flags & (~(WORD_FLAG_NOFREE));
        word->data_pointer = NULL;
        return;
    } else {
        free(word->data_pointer);
        word->data_pointer = NULL;
    }
}

// assigns the contents of word right to word left
void interpreter_word_assign(struct interpreter_word* left, struct interpreter_word* right){
    interpreter_word_clean(left);

    if(right->word_type == WORD_BYTECODE){
        left->bytecode.module = right->bytecode.module;
        left->bytecode.offset = right->bytecode.offset;

        left->array_length = right->array_length;
        left->word_type = right->word_type;
    } else if(right->word_type == WORD_STRING){
        left->data_pointer = malloc(strlen(right->data_pointer) + 1);
        strcpy(left->data_pointer, right->data_pointer);
    } else if(right->word_type == WORD_LIST){
        // TODO: do references in here
        left->data_pointer = right->data_pointer;
    } else {
        //left->data_pointer = right->data_pointer;
        int memsize = word_length(right->word_type) * right->array_length;
        left->data_pointer = malloc(memsize);
        memcpy(left->data_pointer, right->data_pointer, memsize);
    }

    left->array_length = right->array_length;
    left->word_type = right->word_type;
}

void interpreter_word_print(word_index word){
    struct interpreter_word* w = interpreter_words + word;

    switch(w->word_type){
        case WORD_VARIABLE:
            printf("NULL ");
            break;
        case WORD_BYTECODE:
            printf("Bytecode, module %d offset %d ", w->bytecode.module, w->bytecode.offset);
            break;
        case WORD_LIST:
            printf("( list ");
            struct interpreter_list_element* e = interpreter_lists[(list_index)w->data_pointer].first;
            for(; e; e = e->next){
                interpreter_word_print(e->word);
            }
            printf(")");
            break;
        case WORD_STRING:
            printf("\"%s\"", (char*)w->data_pointer);
            break;
        case WORD_FLOAT32:
            printf("F32 [%.1f", *((float*)w->data_pointer));
            if(w->array_length > 1)
                for(int i = 1; i < w->array_length; i++)
                    printf(", %.1f", *(((float*)w->data_pointer) + i));
            printf("]");
            break;
        case WORD_UINT8:
            printf("UInt8 [%d", *((unsigned char*)w->data_pointer));
            if(w->array_length > 1)
                for(int i = 1; i < w->array_length; i++)
                    printf(", %d", *(((unsigned char*)w->data_pointer) + i));
            printf("]");
            break;
        default:
            printf("Unrecognized (%d)", w->word_type);
    }
}

struct interpreter_state {
    int state;
    unsigned short module;
    unsigned short bytecode_offset;
};

// i think these are for returning from functions and stuff???
int INTERPRETER_STATE = 0;
//int interpreter_state_stack[100] = {0};
struct interpreter_state interpreter_state_stack[100];
int interpreter_state_last = 0;
int interpreter_position = 0; // ???

unsigned int interpreter_stack[100] = {0};
int interpreter_stack_last = 0; // actually it's one past the last

void interpreter_state_push(int new_state, unsigned short module, unsigned short bytecode_offset){
    interpreter_state_last++;
    interpreter_state_stack[interpreter_state_last].state = new_state;
    interpreter_state_stack[interpreter_state_last].module = module;
    interpreter_state_stack[interpreter_state_last].bytecode_offset = bytecode_offset;
    INTERPRETER_STATE = new_state;
}

struct interpreter_state interpreter_state_pop(){
    struct interpreter_state ret = interpreter_state_stack[interpreter_state_last];
    interpreter_state_last--;
    INTERPRETER_STATE = interpreter_state_stack[interpreter_state_last].state;
    return ret;
}

void interpreter_stack_push(word_ref reference){
    if(interpreter_stack_last >= 100) {clogg(CLOG_INTERPRET, CLOG_ERROR, "Attempting to push on a full stack; ignoring."); return;}
    interpreter_words[reference].refs++;
    interpreter_stack[interpreter_stack_last] = reference;
    interpreter_stack_last++;
}

word_ref interpreter_stack_pop(){
    if(interpreter_stack_last == 0) {clogg(CLOG_INTERPRET, CLOG_ERROR, "Attempting to pop an empty stack; ignoring."); return 0;}
    interpreter_stack_last--;
    interpreter_words[interpreter_stack[interpreter_stack_last]].refs--;
    return interpreter_stack[interpreter_stack_last];
}

// TODO: make this code better
void interpret(struct intepreter_module* mod, unsigned short module_index, unsigned short bytecode_offset){
    unsigned char* bytecode = mod->bytecode;
    unsigned char* literals = mod->literals;

    void swap_module(unsigned short index){
        module_index = index;
        mod = &interpreter_modules[index];
        bytecode = mod->bytecode;
        literals = mod->literals;
    }

    for(int i = bytecode_offset; i < mod->bytecode_last; i++){
        unsigned short instr = bytecode[i];
        unsigned short param = *((unsigned short*)(&bytecode[i+1]));
        switch(instr){
        case INTRPR_EXECUTEWORD:
            if(interpreter_words[param].word_type != WORD_BYTECODE){
                printf("Pushing word to stack %d\n", param);
                interpreter_stack_push(param);
                i++; i++;
            } else {
                printf("Executing word %d\n", param);
                i++; i++;
                interpreter_state_push(0, module_index, i);
                i = interpreter_words[param].bytecode.offset - 1;
                swap_module(interpreter_words[param].bytecode.module);
            }
            break;
        case INTRPR_STACKEXECUTECOND:
            word_index ddd = interpreter_stack_pop();
            struct interpreter_word* exec = interpreter_words + ddd;
            struct interpreter_word* cond = interpreter_words + interpreter_stack_pop();

            if(cond->word_type != WORD_UINT8) {clogg(CLOG_INTERPRET, CLOG_ERROR, "Conditional only accept UINT8 as param!"); return;}

            if(*((unsigned char*)(cond->data_pointer)) == 1){
                printf("Condition TRUE\n");
                if(exec->word_type != WORD_BYTECODE){
                    printf("Pushing word to stack %d\n", ddd);
                    interpreter_stack_push(ddd);
                } else {
                    printf("Executing word %d\n", ddd);
                    interpreter_state_push(0, module_index, i);
                    i = exec->bytecode.offset - 1;
                    swap_module(exec->bytecode.module);
                }
            } else printf("Condition FALSE\n");
            break;
        case INTRPR_JUMP:
            printf("Jumping to %d\n", param);
            i = param - 1;
            break;
        case INTRPR_RETURN:
            struct interpreter_state state = interpreter_state_pop();
            i = state.bytecode_offset;
            swap_module(state.module);
            printf("Returning to %d\n", i);
            break;
        case INTRPR_DROP:
            interpreter_stack_pop();
            break;
        case INTRPR_DUPLICATE:
            {
                word_ref right = interpreter_stack_pop();

                interpreter_stack_push(right);
                interpreter_stack_push(right);
            }
            break;
        case INTRPR_SWAP:
            {
                word_ref right = interpreter_stack_pop();
                word_ref left = interpreter_stack_pop();

                interpreter_stack_push(right);
                interpreter_stack_push(left);
            }
            break;
        case INTRPR_OVER:
            {
                word_ref right = interpreter_stack_pop();
                word_ref left = interpreter_stack_pop();

                interpreter_stack_push(left);
                interpreter_stack_push(right);
                interpreter_stack_push(left);
            }
            break;
        case INTRPR_ROTATE:
            {
                word_ref right = interpreter_stack_pop();
                word_ref middle = interpreter_stack_pop();
                word_ref left = interpreter_stack_pop();

                interpreter_stack_push(middle);
                interpreter_stack_push(right);
                interpreter_stack_push(left);
            }
            break;
        case INTRPR_PUSH_LIST:
            {
                word_ref ref = insert_word_variable(WORD_LIST, interpreter_new_list());
                interpreter_stack_push(ref);
            }
            break;
        case INTRPR_LISTAPPEND:
            {
            word_ref word = interpreter_stack_pop();
            word_ref list = interpreter_stack_pop();

            interpreter_list_append(interpreter_words[list].data_pointer, word);
            }
            break;
        case INTRPR_PUSHFLOAT32:
            i++;
            printf("Pushing float on stack %f\n", *((float*)(literals + *((unsigned short*)(bytecode + i)))));
            float* benor = malloc(sizeof(float));
            *benor = *((float*)(literals + *((unsigned short*)(bytecode + i))));
            unsigned short ref = insert_word_variable(WORD_FLOAT32, benor);
            interpreter_stack_push(ref);
            i++;
            break;
        case INTRPR_PUSH_STRING:
            {
                i++;
                printf("Pushing string on stack %s\n", literals + *((unsigned short*)(bytecode + i)));
                char* benor = malloc(strlen((char*)literals + *((unsigned short*)(bytecode + i))) + 1);
                strcpy(benor, (char*)literals + *((unsigned short*)(bytecode + i)));
                unsigned short ref = insert_word_variable(WORD_STRING, benor);
                interpreter_stack_push(ref);
                i++;
            }
            break;
        case INTRPR_PUSH_LAMBDA:
            {
                i++;
                printf("Pushing lambda on stack %d\n", *((unsigned short*)(bytecode + i)));
                //unsigned short ref = insert_word_variable(WORD_BYTECODE, 0);
                //interpreter_words[ref].bytecode.offset = *((unsigned short*)(bytecode + i));
                //interpreter_words[ref].bytecode.offset = param;
                unsigned short ref = insert_word_bytecode(WORD_BYTECODE, module_index, param);
                interpreter_stack_push(ref);
                i++;
            }
            break;
        case INTRPR_ASSIGN:
            {
                struct interpreter_word* right = interpreter_words + interpreter_stack_pop();
                struct interpreter_word* left = interpreter_words + interpreter_stack_pop();

                interpreter_word_assign(left, right);
            }
            break;
        case INTRPR_TEST1:
            printf("Stack dump:");
            for (int i = 0; i < interpreter_stack_last; i++){
                printf(" [%d | %s]", interpreter_stack[i], word_type_to_str(interpreter_words[interpreter_stack[i]].word_type));
            }
            printf("\n");
            break;
        case INTRPR_TEST2:
            {
                printf("Infoprint: ");
                word_ref word = interpreter_stack_pop();
                interpreter_word_print(word);
                printf("\n");
            }
            break;
        case INTRPR_VECTORIZE:
            {
                unsigned short pams[100] = {0}; // this means we no more than 100 elem veccy veccy

                // TODO: check if not try to puttin bad type in vector
                int type = WORD_UINT8; // the bborstest type
                for (unsigned short i = 0; i < param; i++){
                    short int w = interpreter_stack_pop();
                    pams[param-i-1] = w;
                    type = word_betterest(type, interpreter_words[w].word_type);
                }

                char* vecmem = malloc(param * word_length(type));

                for(unsigned short i = 0; i < param; i++){
                    struct interpreter_word* w = &interpreter_words[pams[i]];
                    char conv[sizeof(long long)];
                    word_convert(w->word_type, type, w->data_pointer, conv);

                    mempcpy(vecmem + (i * word_length(type)), conv, word_length(type));
                }

                unsigned short b = insert_word_variable(type, vecmem);
                interpreter_words[b].array_length = param;

                interpreter_stack_push(b);
            }
            i++; i++;
            //for (int i = 0; i < 100; i++)printf("i: %d pam: %d\n", i, pams[i]);
            break;
        case INTRPR_EQUAL:
            {
                struct interpreter_word* right = interpreter_words + interpreter_stack_pop();
                struct interpreter_word* left = interpreter_words + interpreter_stack_pop();

                if(right->array_length != left->array_length && right->array_length != 1){
                    printf("Can't compare different length arrays. C line %d @ %s\n", __LINE__, __FUNCTION__);
                    return;
                }

                int new_type = word_betterest(left->word_type, right->word_type);
                if(word_is_int(new_type)) new_type = WORD_INT64;
                else if(word_is_uint(new_type)) new_type = WORD_UINT64;
                else if(word_is_float(new_type)) new_type = WORD_FLOAT64;
                else printf("WTF???");

                unsigned char* new_val = malloc(sizeof(unsigned char));
                unsigned int new_word = insert_word_variable(WORD_UINT8, new_val);

                char isequal = 1;

                for (int i = 0; i < left->array_length; i++){
                    char left_conv[sizeof(long long)];
                    char right_conv[sizeof(long long)];

                    word_convert(left->word_type, new_type, left->data_pointer + (word_length(left->word_type) * i), left_conv);
                    word_convert(right->word_type, new_type, right->data_pointer + (word_length(right->word_type) * i), right_conv);

                    switch(new_type){
                    case WORD_INT64:
                        isequal = *((signed long*)left_conv) == *((signed long*)right_conv);
                        break;
                    case WORD_UINT64:
                        isequal = *((unsigned long*)left_conv) == *((unsigned long*)right_conv);
                        break;
                    case WORD_FLOAT64:
                        isequal = *((double*)left_conv) == *((double*)right_conv);
                        break;
                    }

                    if (!isequal) break;
                }

                *new_val = isequal;
                interpreter_stack_push(new_word);
            }
            break;
        case INTRPR_INVERT:
            {
                struct interpreter_word* val = interpreter_words + interpreter_stack_pop();

                if(val->array_length != 1){
                    printf("Can't invert an array. Code line %d @ %s\n", __LINE__, __FUNCTION__);
                    return;
                }

                if(val->word_type != WORD_UINT8){
                    printf("Can invert only logicals. Code line %d @ %s\n", __LINE__, __FUNCTION__);
                    return;
                }

                unsigned char* new_val = malloc(sizeof(unsigned char));
                unsigned int new_word = insert_word_variable(WORD_UINT8, new_val);

                *new_val = *((unsigned char*)val->data_pointer) ? 0 : 1;
                interpreter_stack_push(new_word);
            }
            break;
        case INTRPR_ADDITION:
            // TODO: do type checking -- if try plussing bytecode and float no good
            struct interpreter_word* right = interpreter_words + interpreter_stack_pop();
            struct interpreter_word* left = interpreter_words + interpreter_stack_pop();

            if(right->array_length != left->array_length && right->array_length != 1){
                printf("Can't add different length arrays. C line %d @ %s\n", __LINE__, __FUNCTION__);
                //printf("r: %d l: %d\n", right->array_length, left->array_length);
                return;
            }

            int new_type = word_betterest(left->word_type, right->word_type);

            // allocate & create the new word
            void* new_val = malloc(word_length(new_type) * left->array_length);
            unsigned int new_word = insert_word_variable(new_type, new_val);
            interpreter_words[new_word].array_length = left->array_length;

            for (int i = 0; i < left->array_length; i++){
                int k = right->array_length == 1 ? 0 : i;

                char left_conv[sizeof(long long)];  // idk how long this should be
                char right_conv[sizeof(long long)];

                word_convert(left->word_type, new_type, left->data_pointer + (word_length(left->word_type) * i), left_conv);
                word_convert(right->word_type, new_type, right->data_pointer + (word_length(right->word_type) * k), right_conv);

                switch(new_type){
                case WORD_INT8:
                    *((signed char*)new_val + i) = *((signed char*)left_conv) + *((signed char*)right_conv);
                    break;
                case WORD_INT16:
                    *((signed short*)new_val + i) = *((signed short*)left_conv) + *((signed short*)right_conv);
                    break;
                case WORD_INT32:
                    *((signed int*)new_val + i) = *((signed int*)left_conv) + *((signed int*)right_conv);
                    break;
                case WORD_INT64:
                    *((signed long*)new_val + i) = *((signed long*)left_conv) + *((signed long*)right_conv);
                    break;
                case WORD_UINT8:
                    *((unsigned char*)new_val + i) = *((unsigned char*)left_conv) + *((unsigned char*)right_conv);
                    break;
                case WORD_UINT16:
                    *((unsigned short*)new_val + i) = *((unsigned short*)left_conv) + *((unsigned short*)right_conv);
                    break;
                case WORD_UINT32:
                    *((unsigned int*)new_val + i) = *((unsigned int*)left_conv) + *((unsigned int*)right_conv);
                    break;
                case WORD_UINT64:
                    *((unsigned long*)new_val + i) = *((unsigned long*)left_conv) + *((unsigned long*)right_conv);
                    break;
                case WORD_FLOAT8:
                    printf("FLOAT8 not implemented.\n");
                    break;
                case WORD_FLOAT16:
                    printf("FLOAT16 not implemented.\n");
                    break;
                case WORD_FLOAT32:
                    *((float*)new_val + i) = *((float*)left_conv) + *((float*)right_conv);
                    break;
                case WORD_FLOAT64:
                    *((float*)new_val + i) = *((float*)left_conv) + *((float*)right_conv);
                    break;
                }
            }

            interpreter_stack_push(new_word);
            break;
        case INTRPR_SUBTRACTION:
            {


            // TODO: do type checking -- if try plussing bytecode and float no good
            unsigned int right = interpreter_stack_pop();
            unsigned int left = interpreter_stack_pop();

            float* rightf = interpreter_words[right].data_pointer;
            float* leftf = interpreter_words[left].data_pointer;

            float* newf = malloc(sizeof(float));

            printf("Minussing %f and also %f\n", *leftf, *rightf);

            *newf = *leftf - *rightf;

            unsigned int newr = insert_word_variable(WORD_FLOAT32, newf);

            interpreter_stack_push(newr);
            }
            break;
        case INTRPR_NATIVECALL:
            {
                struct interpreter_word* w = interpreter_words + interpreter_stack_pop();
                unsigned long index;

                word_convert(w->word_type, WORD_UINT64, w->data_pointer, &index);

                //static const unsigned long max_calls = sizeof(call_fs) / sizeof(void*);
                if(index >= max_call_fs) {clogg(CLOG_INTERPRET, CLOG_ERROR, "Attempting to do an invalid call; ignoring."); return;}

                call_fs[index]();
            }
            break;
        default:
            printf("Invalid instruction number %u @ %d\n", instr, i);
        }



    }
}

void interpreter_module_clear(struct intepreter_module* mod){
    for (struct link_in* i = mod->first_in; i != NULL; i = i->next) free(i);
    for (struct link_in* i = mod->last_in; i != NULL; i = i->next) free(i);
    if (mod->literals) free(mod->literals);
    if (mod->bytecode) free(mod->bytecode);
    mod->first_in = NULL;
    mod->last_in = NULL;
    mod->literals = NULL;
    mod->bytecode = NULL;
    mod->literals_last = 0;
    mod->bytecode_last = 0;
    mod->literals = malloc(INTERPRETER_MAX_LITERALS);
    mod->bytecode = malloc(INTERPRETER_MAX_BYTECODE);
}

void link(struct intepreter_module* mod){
    struct link_in* in = mod->first_in;



    while(in){
        unsigned short handle = find_word((unsigned char*)&mod->literals[in->word_name]);
        if(handle == 65535){ // -1 of a unsigned short aka handle not found
            /*printf("Name %s not found, unable to link!\n", &literals[in->word_name]);*/
            printf("Word %s not found, creating...", &mod->literals[in->word_name]);
            handle = insert_word_variable(WORD_VARIABLE, NULL); // is WORD_VARIABLE ok?
            insert_handle((char*)&mod->literals[in->word_name], handle); // TODO: copy the string instead of referencing
            interpreter_words[handle].flags |= WORD_FLAG_NOCOLLECT;
            printf(" done.\n");
        }

        printf("Linking word %s...\n", &mod->literals[in->word_name]);
        unsigned short* putte = (unsigned short*)(&mod->bytecode[in->code_offset]);
        *putte = handle;

        in = in->next;
    }


}
