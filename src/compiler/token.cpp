#include <compiler/token.h>
#include <string.h>

namespace ligma {

token_t GetTokenFromString (const char* string) {
    switch (*string) {
        case '\n': return TOKEN_NEWLINE;
        
        case '(': return TOKEN_OPEN_BLOCK;
        case ')': return TOKEN_CLOSE_BLOCK;
        
        case '[': return TOKEN_OPEN_PUSH_BLOCK;
        case ']': return TOKEN_CLOSE_PUSH_BLOCK;
        
        case '+': if (*(string+1) == '\0') return TOKEN_OPERATOR_ADD; break;
        case '-': if (*(string+1) == '\0') return TOKEN_OPERATOR_SUB; break;
        case '*': return TOKEN_OPERATOR_MUL;
        case '/': return TOKEN_OPERATOR_DIV;
        
        case '`': return TOKEN_STRING_LITERAL;
        case '"': return TOKEN_STRING_LITERAL;
        
        case '.':
            switch (*(string+1)) {
                case 's': return TOKEN_OPERATOR_PRINTSTACK;
                case '.': return TOKEN_OPERATOR_PRINTINFO;
                case '\0': return TOKEN_OPERATOR_PRINT;
            } break;
        
        case '=': 
            switch (*(string+1)) {
                case '=': return TOKEN_OPERATOR_EQUAL;
                case '\0': return TOKEN_OPERATOR_IDENTITY;
            } break;
        case '!':
            switch (*(string+1)) {
                case '=': return TOKEN_OPERATOR_NOTEQUAL;
            } break;
        case '>':
            switch (*(string+1)) {
                case '=': return TOKEN_OPERATOR_GREATEROREQUAL;
                case '>': return TOKEN_OPERATOR_VECTOREXTRACT;
                case '\0': return TOKEN_OPERATOR_GREATERTHAN;
            } break;
        case '<':
            switch (*(string+1)) {
                case '=': return TOKEN_OPERATOR_LESSEROREQUAL;
                case '<': return TOKEN_OPERATOR_VECTORINSERT;
                case '\0': return TOKEN_OPERATOR_LESSERTHAN;
            } break;
            
            
        case 'a':
            if (strcmp ("and", string) == 0) return TOKEN_OPERATOR_AND;
            break;
        case 'c':
            if (strcmp ("cr", string) == 0) return TOKEN_OPERATOR_CARRIAGE_RETURN;
            break;
        case 'd':
            if (strcmp ("data", string) == 0) return TOKEN_OPERATOR_LISTDATA;
            if (strcmp ("declare", string) == 0) return TOKEN_DECLARE_BLOCK;
            if (strcmp ("do", string) == 0) return TOKEN_OPERATOR_EXECUTE;
            if (strcmp ("dup", string) == 0) return TOKEN_OPERATOR_DUPLICATE;
            if (strcmp ("drop", string) == 0) return TOKEN_OPERATOR_DROP;
            break;
        case 'e':
            if (strcmp ("else", string) == 0) return TOKEN_CONTROL_ELSE;
            if (strcmp ("explode", string) == 0) return TOKEN_CONTROL_EXPLODE;
            break;
        case 'f':
            if (strcmp ("fail", string) == 0) return TOKEN_FAIL;
            if (strcmp ("false", string) == 0) return TOKEN_FALSE;
            if (strcmp ("floor", string) == 0) return TOKEN_OPERATOR_FLOOR;
            break;
        case 'i':
            if (strcmp ("if", string) == 0) return TOKEN_CONTROL_IF;
            if (strcmp ("is", string) == 0) return TOKEN_OPERATOR_IDENTITY;
            break;
        case 'l':
            if (strcmp ("lambda", string) == 0) return TOKEN_LAMBDA_BLOCK;
            if (strcmp ("len", string) == 0) return TOKEN_OPERATOR_LENGTH;
            if (strcmp ("list", string) == 0) return TOKEN_LIST_BLOCK;
            break;
        case 't':
            if (strcmp ("type", string) == 0) return TOKEN_OPERATOR_TYPE;
            if (strcmp ("true", string) == 0) return TOKEN_TRUE;
            break;
        case 'm':
            if (strcmp ("maybe", string) == 0) return TOKEN_MAYBE;
            if (strcmp ("mod", string) == 0) return TOKEN_OPERATOR_MODULO;
            break;
        case 'n':
            if (strcmp ("next", string) == 0) return TOKEN_OPERATOR_LISTNEXT;
            if (strcmp ("nil", string) == 0) return TOKEN_NIL;
            if (strcmp ("not", string) == 0) return TOKEN_OPERATOR_NOT;
            break;
        case 'o':
            if (strcmp ("or", string) == 0) return TOKEN_OPERATOR_OR;
            if (strcmp ("ok", string) == 0) return TOKEN_OK;
            if (strcmp ("over", string) == 0) return TOKEN_OPERATOR_OVER;
            break;
        case 'p':
            if (strcmp ("print", string) == 0) return TOKEN_OPERATOR_PRINT;
            break;
        case 's':
            if (strcmp ("set", string) == 0) return TOKEN_OPERATOR_ASSIGN;
            if (strcmp ("setnext", string) == 0) return TOKEN_OPERATOR_LISTNEXTSET;
            if (strcmp ("setdata", string) == 0) return TOKEN_OPERATOR_LISTDATASET;
            if (strcmp ("swap", string) == 0) return TOKEN_OPERATOR_SWAP;
            break;
        case 'r':
            if (strcmp ("repeat", string) == 0) return TOKEN_CONTROL_REPEAT;
            if (strcmp ("return", string) == 0) return TOKEN_CONTROL_RETURN;
            if (strcmp ("rot", string) == 0) return TOKEN_OPERATOR_ROTATE;
            break;
        case 'v':
            if (strcmp ("vec", string) == 0) return TOKEN_VECTOR_BLOCK;
            break;
        default:
            break;
    }
    
    if ((*string >= '0' && *string <= '9') || *string == '-' || *string == '+') {
        for (const char* c = string; *c != '\0'; c++) {
            if (*c == '.') return TOKEN_FLOAT_LITERAL;
        }
        
        return TOKEN_INT_LITERAL;
    }
    
    return TOKEN_WORD;
}

token_t GetNextTokenFromString (const char* string) {
    switch (*string) {
        case '(': return TOKEN_OPEN_BLOCK;
        case ')': return TOKEN_CLOSE_BLOCK;
        case 'd':
            if (strncmp("declare", string, 7) == 0) return TOKEN_DECLARE_BLOCK;
            break;
        case 'e': 
            if (strncmp("else", string, 4) == 0) {
                return TOKEN_CONTROL_ELSE;
            }
            break;
        case 'l':
            if (strncmp("lambda", string, 6) == 0) return TOKEN_LAMBDA_BLOCK;
            if (strncmp("list ", string, 5) == 0) return TOKEN_LIST_BLOCK;
            break;
        case 'v':
            if (strncmp("vec", string, 3) == 0) return TOKEN_VECTOR_BLOCK;
            break;
        default:
            break;
    }
    
    return TOKEN_NONE;
}
}