// intrpr_parser.c
// No copyright intended.
//
// Contains the code that converts tokens into bytecodes.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intrpr_structures.h>
#include <intrpr_parser.h>
#include <intrpr_word_types.h>
#include <intrpr_instructions.h>

struct intepreter_module* PARSER_MODULE;
unsigned short PARSER_MODULE_INDEX;

int PARSER_STATE = 0;
int parser_state_stack[100] = {0};
int parser_state_last = 0;

void parser_mount_module(struct intepreter_module* mod, unsigned short index){
    // reset parser state
    PARSER_STATE = 0;
    parser_state_last = 0;

    // set the parser module
    PARSER_MODULE = mod;
    PARSER_MODULE_INDEX = index;
}

void parser_state_push(int new_state, int bytecode_offset){
    // the state which will be reinstated upon popping
    parser_state_stack[parser_state_last] = PARSER_STATE;
    parser_state_last++;
    // bytecode offset which will be return upon popping
    parser_state_stack[parser_state_last] = bytecode_offset;
    parser_state_last++;
    PARSER_STATE = new_state;
}

int parser_state_pop(){
    parser_state_last--;
    int return_off = parser_state_stack[parser_state_last];
    parser_state_last--;
    PARSER_STATE = parser_state_stack[parser_state_last];
    return return_off;
}




// wtf is this and do we need this??
struct raw_word {
    unsigned short word_name;
    struct link_in* links;
    unsigned char code[1000];
};






void bytecode_append_uchar(unsigned char appendable){
    PARSER_MODULE->bytecode[PARSER_MODULE->bytecode_last] = appendable;
    PARSER_MODULE->bytecode_last++;
}

void bytecode_append_ushort(unsigned short appendable){
    unsigned short* place = (unsigned short*)&PARSER_MODULE->bytecode[PARSER_MODULE->bytecode_last];
    *place = appendable;
    PARSER_MODULE->bytecode_last += 2;
}

void bytecode_insert_ushort(unsigned short insertable, int offset){
    unsigned short* place = (unsigned short*)&PARSER_MODULE->bytecode[PARSER_MODULE->bytecode_last];
    *place = insertable;
}

int emit_append_literal(void* appendable, int length){
    int offset = PARSER_MODULE->literals_last;
    if(offset + length + 1 > INTERPRETER_MAX_LITERALS){
        printf("Appending literal overflow!\n");
        abort();
    }

    memcpy((PARSER_MODULE->literals + PARSER_MODULE->literals_last), appendable, length);
    PARSER_MODULE->literals_last = PARSER_MODULE->literals_last + 1 + length;
    return offset;
}

// TODO: check if this function could be just a wrapper for emit_append_literal
int emit_append_string_literal(const char* const literal){
    int offset = PARSER_MODULE->literals_last;
    int length = strlen(literal);
    if(offset + length + 1 > INTERPRETER_MAX_LITERALS){
        printf("Appending literal overflow!\n");
        abort();
    }

    strcpy((char*)(PARSER_MODULE->literals + PARSER_MODULE->literals_last), literal);
    PARSER_MODULE->literals_last = PARSER_MODULE->literals_last + 1 + length;
    return offset;
}

void emit_link_in(int str_literal_off, int bytecode_off){
    struct link_in* in = malloc(sizeof(struct link_in));
    in->next = NULL;
    in->code_offset = bytecode_off;
    in->word_name = str_literal_off;

    if(PARSER_MODULE->last_in){
        PARSER_MODULE->last_in->next = in;
        PARSER_MODULE->last_in = in;
    } else {
        PARSER_MODULE->first_in = in;
        PARSER_MODULE-> last_in = in;
    }
}

// TODO: this function needs to be yeeted into oblivion.
// and also merged with the token thing function
void compile(const char* buffer, int instr){
    //if(instr < 256) printf("instruction: %s\n", instr_str[instr]);
    //else printf("instruction: %d\n", instr);

    switch(instr){
    case INTRPR_STARTBLOCK:
        parser_state_push(INTRPR_PARSE_BLOCKSTARTED, PARSER_MODULE->bytecode_last);
        break;
    case INTRPR_VARIABLEBLOCK:
        parser_state_push(INTRPR_PARSE_DEFVARNAME, PARSER_MODULE->bytecode_last);
        break;
    case INTRPR_WORDNAME:
        bytecode_append_uchar(INTRPR_EXECUTEWORD);
        emit_link_in(emit_append_string_literal(buffer), PARSER_MODULE->bytecode_last);
        bytecode_append_ushort(0);
        parser_state_pop();
        break;
    case INTRPR_WORDLITERAL:
        bytecode_append_uchar(INTRPR_EXECUTEWORD);

        emit_link_in(emit_append_string_literal(buffer), PARSER_MODULE->bytecode_last);

        bytecode_append_ushort(0);
        break;
    case INTRPR_STRINGLITERAL:
        bytecode_append_uchar(INTRPR_PUSH_STRING);
        //bytecode_append_ushort(emit_append_literal(buffer, strlen(buffer) + 1));
        bytecode_append_ushort(emit_append_string_literal(buffer));
        break;
    case INTRPR_LAMBDABLOCK:
        parser_state_pop();

        bytecode_append_uchar(INTRPR_PUSH_LAMBDA);
        bytecode_append_ushort(PARSER_MODULE->bytecode_last + 2 + 1 + 2);

        bytecode_append_uchar(INTRPR_JUMP);

        parser_state_push(INTRPR_PARSE_LAMBDA, PARSER_MODULE->bytecode_last);
        bytecode_append_ushort(0);

        break;
    case INTRPR_LISTBLOCK:
        parser_state_pop();

        bytecode_append_uchar(INTRPR_PUSH_LIST);

        parser_state_push(INTRPR_PARSE_LIST, PARSER_MODULE->bytecode_last);

        break;
    case INTRPR_NUMBERLITERAL:
        float num = atof(buffer);
        int loc = emit_append_literal(&num, sizeof(float));

        bytecode_append_uchar(INTRPR_PUSHFLOAT32);
        bytecode_append_ushort(loc);
        break;
    case INTRPR_ENDBLOCK:
        if(PARSER_STATE == INTRPR_PARSE_LAMBDA){
            bytecode_append_uchar(INTRPR_RETURN);
            *((short int*)(&PARSER_MODULE->bytecode[parser_state_pop()])) = PARSER_MODULE->bytecode_last;
        } else {
            parser_state_pop();
        }

        break;
    case INTRPR_TEST1:
        bytecode_append_uchar(INTRPR_TEST1);
        break;
    case INTRPR_TEST2:
        bytecode_append_uchar(INTRPR_TEST2);
        break;

    case INTRPR_VECTORIZE:
        {
            char f[5];
            strncpy(f, buffer + 3, 5);  // stinky

            bytecode_append_uchar(INTRPR_VECTORIZE);
            bytecode_append_ushort(atoi(f));
        }
        break;
    case INTRPR_STACKEXECUTECOND:
        bytecode_append_uchar(INTRPR_STACKEXECUTECOND);
        break;
    case INTRPR_ASSIGN:
        bytecode_append_uchar(INTRPR_ASSIGN);
        break;
    case INTRPR_EQUAL:
        bytecode_append_uchar(INTRPR_EQUAL);
        break;
    case INTRPR_INVERT:
        bytecode_append_uchar(INTRPR_INVERT);
        break;
    case INTRPR_ADDITION:
        bytecode_append_uchar(INTRPR_ADDITION);
        break;
    case INTRPR_SUBTRACTION:
        bytecode_append_uchar(INTRPR_SUBTRACTION);
        break;
    case INTRPR_MULTIPLICATION:
        bytecode_append_uchar(INTRPR_MULTIPLICATION);
        break;
    case INTRPR_DIVISION:
        bytecode_append_uchar(INTRPR_DIVISION);
        break;
    case INTRPR_LISTAPPEND:
        bytecode_append_uchar(INTRPR_LISTAPPEND);
        break;
    case INTRPR_DUPLICATE:
        bytecode_append_uchar(INTRPR_DUPLICATE);
        break;
    case INTRPR_ROTATE:
        bytecode_append_uchar(INTRPR_ROTATE);
        break;
    case INTRPR_SWAP:
        bytecode_append_uchar(INTRPR_SWAP);
        break;
    case INTRPR_OVER:
        bytecode_append_uchar(INTRPR_OVER);
        break;
    case INTRPR_DROP:
        bytecode_append_uchar(INTRPR_DROP);
        break;
    case INTRPR_NATIVECALL:
        bytecode_append_uchar(INTRPR_NATIVECALL);
        break;

    default:
        break;
    }
}


void parse_token(const char* buffer){
    static char comment = 0;

    if (strcmp(buffer, ";") == 0){
        comment = comment ? 0 : 1;
        return;
    } else if (comment) return;

    int instr;

    if (PARSER_STATE == INTRPR_PARSE_NORMAL || PARSER_STATE == INTRPR_PARSE_BLOCKSTARTED || PARSER_STATE == INTRPR_PARSE_LAMBDA || PARSER_STATE == INTRPR_PARSE_LIST){

        if(strcmp(buffer, "(") == 0)
            instr = INTRPR_STARTBLOCK;
        else if(strcmp(buffer, ")") == 0)
            instr = INTRPR_ENDBLOCK;
        else if(strcmp(buffer, "word") == 0)
            instr = INTRPR_WORDBLOCK;
        else if(strcmp(buffer, "list") == 0)
            instr = INTRPR_LISTBLOCK;
        else if(strcmp(buffer, "lambda") == 0)
            instr = INTRPR_LAMBDABLOCK;
        else if(strcmp(buffer, "set") == 0)
            instr = INTRPR_ASSIGN;
        else if(strcmp(buffer, "variable") == 0)
            instr = INTRPR_VARIABLEBLOCK;
//        else if(strcmp(buffer, "vector") == 0)
//            instr = VECTORBLOCK;
        else if(strcmp(buffer, ".s") == 0)
            instr = INTRPR_TEST1;
        else if(strcmp(buffer, "print") == 0)
            instr = INTRPR_TEST2;
        else if(strcmp(buffer, "set") == 0)
            instr = INTRPR_ASSIGN;
        else if(strcmp(buffer, "+") == 0)
            instr = INTRPR_ADDITION;
        else if(strcmp(buffer, "-") == 0)
            instr = INTRPR_SUBTRACTION;
        else if(strcmp(buffer, "=") == 0)
            instr = INTRPR_EQUAL;
        else if(strcmp(buffer, "drop") == 0)
            instr = INTRPR_DROP;
        else if(strcmp(buffer, "dup") == 0)
            instr = INTRPR_DUPLICATE;
        else if(strcmp(buffer, "swap") == 0)
            instr = INTRPR_SWAP;
        else if(strcmp(buffer, "rot") == 0)
            instr = INTRPR_ROTATE;
        else if(strcmp(buffer, "over") == 0)
            instr = INTRPR_OVER;
        else if(strcmp(buffer, "invert") == 0)
            instr = INTRPR_INVERT;
        else if(strcmp(buffer, "call") == 0)
            instr = INTRPR_NATIVECALL;
        else if(strcmp(buffer, "?") == 0)
            instr = INTRPR_STACKEXECUTECOND;
        else if(strncmp(buffer, "vec", 3) == 0){
            instr = INTRPR_VECTORIZE;
        } else if((buffer[0] >= '0' && buffer[0] <= '9')
                || buffer[0] == '-'
                || buffer[0] == '+'){
            instr = INTRPR_NUMBERLITERAL;
        } else if (buffer[0] == '"'){
            parser_state_push(INTRPR_PARSE_STRING, 0);
            return;
        } else {
            instr = INTRPR_WORDLITERAL;
        }

    } else if (PARSER_STATE == INTRPR_PARSE_DEFVARNAME){
        instr = INTRPR_WORDNAME;
    } else if(PARSER_STATE == INTRPR_PARSE_STRING){
        static char strbuf[200] = {'\0'};

        if(buffer[0] == '"'){
            parser_state_pop();
            instr = INTRPR_STRINGLITERAL;
            compile(strbuf, instr);
            strbuf[0] = '\0';
            goto jumpovercompile;
        } else {
            if (strbuf[0] != '\0') strcat(strbuf, " ");
            strcat(strbuf, buffer);
        }
    }

    compile(buffer, instr);

    jumpovercompile:

    // if building a list, append list building instructions
    if (PARSER_STATE == INTRPR_PARSE_LIST && instr != INTRPR_LISTBLOCK){
            // the way you build a list is by repeatedly creating words on the
            // stack and then appending these words to the the new list
            instr = INTRPR_OVER;
            compile(buffer, instr);
            instr = INTRPR_SWAP;
            compile(buffer, instr);
            instr = INTRPR_LISTAPPEND;
            compile(buffer, instr);
    }
}


