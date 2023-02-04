#include <compiler/compiler.h>
#include <compiler/error.h>
#include <shared/bytecode.h>
#include <shared/instructions.h>
#include <stdlib.h>
#include <string.h>

namespace ligma {

void Compiler::compile_token (token_t token_type, const char* token, token_t next_token) {
    switch (token_type) {
    case TOKEN_WORD: {
        // if declaration mode then do the thing
        offset_t word_name_literal = 0;
        std::map<std::string, offset_t>::iterator it = this->word_map.find(token);
        if (it == this->word_map.end()) {
            word_name_literal = append_string(token);
            this->word_map[token] = word_name_literal;
        } else {
            word_name_literal = it->second;
        }
        
        if (state.get_state() == COMPILE_DECLARATION) {
            offset_t backup = state.get_offset();
            state.pop();
            state.add_local(word_name_literal);
            state.push(COMPILE_DECLARATION, backup);
        } else {
            param_t local;
            if (state.get_state() == COMPILE_PUSH_WORD) {
                state.pop();
                local = state.find_local(word_name_literal);
                state.push(COMPILE_PUSH_WORD, 0);
            } else {
                local = state.find_local(word_name_literal);
            }
            
            if (local == (param_t)-1) {
                append_instr(OPERATION_PUSH_WORD);
                append_symbol(word_name_literal, append_param(-1));
            } else {
                append_instr(OPERATION_PUSH_LOCAL);
                append_param(local);
            }
            
            if (state.get_state() != COMPILE_PUSH_WORD) {
                append_instr(OPERATION_EXECUTE_WORD);
            }
        }

        } break;
    case TOKEN_NIL: 
        append_instr(OPERATION_PUSH_WORD); append_param(0); break;
    case TOKEN_TRUE: 
        append_instr(OPERATION_PUSH_WORD); append_param(1); break;
    case TOKEN_FALSE: 
        append_instr(OPERATION_PUSH_WORD); append_param(2); break;
    case TOKEN_OK: 
        append_instr(OPERATION_PUSH_WORD); append_param(3); break;
    case TOKEN_FAIL: 
        append_instr(OPERATION_PUSH_WORD); append_param(4); break;
    case TOKEN_MAYBE: 
        append_instr(OPERATION_PUSH_WORD); append_param(5); break;
    case TOKEN_FLOAT_LITERAL:
        append_instr(OPERATION_PUSH_FLOAT);
        append_param(append_float(atof(token)));
        break;
    case TOKEN_INT_LITERAL: 
        append_instr(OPERATION_PUSH_INT);
        append_param(append_int(atol(token)));
        break;
    case TOKEN_STRING_LITERAL: 
        append_instr(OPERATION_PUSH_STRING);
        append_param(append_string(token+1));
        break;
    
    case TOKEN_NEWLINE: 
        if (file_counter) {
            append_instr(OPERATION_LINE);
            append_param(++line_counter);
        }
        break;
    
    case TOKEN_OPEN_BLOCK: {
        switch (next_token) {
            case TOKEN_LAMBDA_BLOCK: {
                append_instr(OPERATION_PUSH_LAMBDA);
                offset_t lambda_pos = append_param(-1);
                
                append_instr(OPERATION_JUMP);
                
                offset_t jump_pos = append_param(-1);
                state.push(COMPILE_LAMBDA, jump_pos);
                
                insert_param(lambda_pos, append_instr(OPERATION_NOOP));
                
                if (file_counter) {
                    append_instr(OPERATION_FILE);
                    append_param(file_counter);
                    append_instr(OPERATION_LINE);
                    append_param(line_counter);
                }
                
                } break;
            case TOKEN_LIST_BLOCK:
                state.push(COMPILE_LIST, 0);
                break;
            case TOKEN_VECTOR_BLOCK:
                state.push(COMPILE_VECTOR, 0);
                break;
            case TOKEN_DECLARE_BLOCK:
                append_instr(OPERATION_ALLOCATE_LOCAL);
                state.push(COMPILE_DECLARATION, append_param(-1));
                break;
            default:
                if (state.get_state() == COMPILE_IF || state.get_state() == COMPILE_ELSE) {
                    return;
                }
                
                state.push(COMPILE_BLOCK, 0);
                break;
            }
        } break;
    case TOKEN_CLOSE_BLOCK: {
        if (state.get_local_count() > 0) {
            append_instr(OPERATION_DEALLOCATE_LOCAL);
            append_param(state.get_local_count());
        }
        switch (state.get_state()) {
            case COMPILE_IF:
                if (next_token == TOKEN_CONTROL_ELSE) {
                    offset_t jmp = state.get_offset(); // jump instr parameter generated from if statement
                    state.pop();
                    append_instr(OPERATION_JUMP);
                    offset_t jmp_pos = append_param(-1);
                    state.push(COMPILE_ELSE, jmp_pos);
                    insert_param(jmp, append_instr(OPERATION_NOOP));
                } else {
                    offset_t jmp = state.get_offset();
                    state.pop();
                    insert_param(jmp, append_instr(OPERATION_NOOP));
                }
                break;
            case COMPILE_ELSE:
                insert_param(state.get_offset(), append_instr(OPERATION_NOOP));
                state.pop();
                break;
            case COMPILE_LAMBDA:
                append_instr(OPERATION_RETURN);
                insert_param(state.get_offset(), append_instr(OPERATION_NOOP));
                state.pop();
                break;
            case COMPILE_LIST:
                append_instr(OPERATION_MAKE_LIST);
                append_param(state.get_count() - 2);
                state.pop();
                break;
            case COMPILE_VECTOR:
                append_instr(OPERATION_MAKE_VECTOR);
                append_param(state.get_count() - 2);
                state.pop();
                break;
            case COMPILE_DECLARATION: {
                offset_t decl_count = state.get_offset();
                state.pop();
                insert_param(decl_count, state.get_local_count());
                } break;
            case COMPILE_BLOCK:
                state.pop();
                break;
            default:
                throw CompilerError(IMPROPER_USE_OF_CLOSE_BLOCK);
        }
        } break;
    
    case TOKEN_OPEN_PUSH_BLOCK: 
        state.push(COMPILE_PUSH_WORD, 0); break;
    case TOKEN_CLOSE_PUSH_BLOCK: 
        if (state.get_state() != COMPILE_PUSH_WORD) {
            throw CompilerError(IMPROPER_USE_OF_CLOSE_PUSH);
        }
        state.pop();
        break;
    
    case TOKEN_LAMBDA_BLOCK: break;
    case TOKEN_VECTOR_BLOCK: break;
    case TOKEN_LIST_BLOCK: break;
    case TOKEN_DECLARE_BLOCK: break;
    
    case TOKEN_OPERATOR_ADD: 
        append_instr(OPERATION_ADD); break;
    case TOKEN_OPERATOR_SUB: 
        append_instr(OPERATION_SUB); break;
    case TOKEN_OPERATOR_MUL: 
        append_instr(OPERATION_MUL); break;
    case TOKEN_OPERATOR_DIV: 
        append_instr(OPERATION_DIV); break;
    case TOKEN_OPERATOR_MODULO: 
        append_instr(OPERATION_MOD); break;
    case TOKEN_OPERATOR_FLOOR: 
        append_instr(OPERATION_FLOOR); break;
        
    case TOKEN_OPERATOR_PRINTSTACK: 
        append_instr(OPERATION_PRINTSTACK); break;
    case TOKEN_OPERATOR_PRINTINFO: 
        append_instr(OPERATION_PRINTINFO); break;
    case TOKEN_OPERATOR_PRINT: 
        append_instr(OPERATION_PRINT); break;
    case TOKEN_OPERATOR_CARRIAGE_RETURN: 
        append_instr(OPERATION_CARRIAGE_RETURN); break;
    
    case TOKEN_OPERATOR_LISTNEXT: 
        append_instr(OPERATION_LISTNEXT); break;
    case TOKEN_OPERATOR_LISTDATA: 
        append_instr(OPERATION_LISTDATA); break;
    case TOKEN_OPERATOR_LISTNEXTSET: 
        append_instr(OPERATION_LISTNEXTSET); break;
    case TOKEN_OPERATOR_LISTDATASET: 
        append_instr(OPERATION_LISTDATASET); break;
    
    case TOKEN_OPERATOR_VECTORINSERT: 
        append_instr(OPERATION_VECTORINSERT); break;
    case TOKEN_OPERATOR_VECTOREXTRACT: 
        append_instr(OPERATION_VECTOREXTRACT); break;
    
    case TOKEN_OPERATOR_DROP: 
        append_instr(OPERATION_DROP); break;
    case TOKEN_OPERATOR_OVER: 
        append_instr(OPERATION_OVER); break;
    case TOKEN_OPERATOR_SWAP: 
        append_instr(OPERATION_SWAP); break;
    case TOKEN_OPERATOR_DUPLICATE: 
        append_instr(OPERATION_DUPLICATE); break;
    case TOKEN_OPERATOR_ROTATE: 
        append_instr(OPERATION_ROTATE); break;
    
    case TOKEN_OPERATOR_EQUAL: 
        append_instr(OPERATION_EQUAL); break;
    case TOKEN_OPERATOR_NOTEQUAL: 
        append_instr(OPERATION_NOTEQUAL); break;
    case TOKEN_OPERATOR_GREATERTHAN: 
        append_instr(OPERATION_GREATERTHAN); break;
    case TOKEN_OPERATOR_LESSERTHAN: 
        append_instr(OPERATION_LESSERTHAN); break;
    case TOKEN_OPERATOR_GREATEROREQUAL: 
        append_instr(OPERATION_GREATEROREQUAL); break;
    case TOKEN_OPERATOR_LESSEROREQUAL: 
        append_instr(OPERATION_LESSEROREQUAL); break;
    
    case TOKEN_OPERATOR_AND: 
        append_instr(OPERATION_AND); break;
    case TOKEN_OPERATOR_OR: 
        append_instr(OPERATION_OR); break;
    case TOKEN_OPERATOR_NOT: 
        append_instr(OPERATION_NOT); break;
        
    case TOKEN_OPERATOR_ASSIGN: 
        append_instr(OPERATION_ASSIGN); break;
    case TOKEN_OPERATOR_LENGTH: 
        append_instr(OPERATION_LENGTH); break;
    case TOKEN_OPERATOR_IDENTITY: 
        append_instr(OPERATION_IDENTITY); break;
    case TOKEN_OPERATOR_EXECUTE: 
        append_instr(OPERATION_EXECUTE_WORD); break;
    case TOKEN_OPERATOR_TYPE: 
        append_instr(OPERATION_TYPE); break;

    case TOKEN_CONTROL_IF: {
        if (next_token != TOKEN_OPEN_BLOCK) {
            throw CompilerError(COMPILER_ERROR_NO_BLOCK_START_AFTER_IF);
        }
        append_instr(OPERATION_JUMP_IF_NOT);
        offset_t jmp_pos = append_param(-1);
        state.push(COMPILE_IF, jmp_pos);
        } break;
    case TOKEN_CONTROL_ELSE: 
        if (next_token != TOKEN_OPEN_BLOCK) {
            throw CompilerError(COMPILER_ERROR_NO_BLOCK_START_AFTER_ELSE);
        } else if (state.get_state() != COMPILE_ELSE) {
            throw CompilerError(COMPILER_ERROR_ELSE_WITHOUT_IF);
        }
        // the instruction got already inserted
        break;
    case TOKEN_CONTROL_RETURN: 
        append_instr(OPERATION_RETURN); break;
    case TOKEN_CONTROL_REPEAT: 
        append_instr(OPERATION_JUMP);
        append_param(state.get_offset() + sizeof(param_t));
        break;
    case TOKEN_CONTROL_EXPLODE: 
        append_instr(OPERATION_EXPLODE); break;
    default:
        printf("Token unrecognized: %s\n", token);
        append_instr(OPERATION_NOOP);
    }
    
    state.increment();
    
    // here we should add some checks for states.
    // i.e. if you're making a vector, then you should
    // check if the token you added was a word, etc.
}

inline bool is_seperator (char character) {
    return character == '('
        || character == ')' 
        || character == '[' 
        || character == ']' 
        || character == '"' 
        || character == ';' 
        || character == ' ' 
        || character == '\t' 
        || character == '\n'
        || character == '\0';
}

/// Converts code into tokens and compiles them.
void Compiler::compile (const char* code) {
    const char* c = code;
    char* s = lexing_string;
        
    while (*c) {
        if (is_seperator(*c)) {
            if (*c == '"') {
                do *s++ = *c++; while (*c != '"' && *c != '\0');
                c++;
            } else if (*c == ';') {
                while (*c != '\n' && *c != '\0') c++;
                continue;
            } else {
                *s++ = *c++;
            }
        } else while (*c && !is_seperator(*c)) {            
            *s = *c;
        
            s++;
            c++;
        }
        
        *s = '\0';
        while (*c == ' ' || *c == '\t') c++;
        
        // honestly, this function is total garbage.
        // just look at this:
        token_t token = GetTokenFromString (lexing_string);
        token_t next_token = GetNextTokenFromString (c);
        
        compile_token (token, lexing_string, next_token);
        
        s = lexing_string;
    }
}

offset_t Compiler::append_instr (instr_t instr) {
    return this->bytecode->bytecode.append(instr);
}

offset_t Compiler::append_param (param_t param) {
    return this->bytecode->bytecode.append(param);
}

offset_t Compiler::insert_param (offset_t offset, param_t param) {
    return this->bytecode->bytecode.insert(offset, param);
}

offset_t Compiler::append_float (float value) {
    return this->bytecode->literals.append(value);
}

offset_t Compiler::append_int (int value) {
    return this->bytecode->literals.append(value);
}

offset_t Compiler::append_string (const char* str) {
    return this->bytecode->literals.append(str, strlen(str) + 1);
}

void Compiler::append_symbol (offset_t name, offset_t offset) {
    this->bytecode->symbols.append(name);
    this->bytecode->symbols.append(offset);
}

/// Finds the file name of currently compiling code.
/// @return A null-terminated string to the file name or NULL if there is no file.
const char* Compiler::get_file_name() {
    if (!file_counter) {
        return NULL;
    }
    
    return this->bytecode->literals.begin() + file_counter;
}

/// Finds the line number of currently compiling code.
/// @return Line number or 0 if there is no file.
offset_t Compiler::get_line_number() {
    return line_counter;
}


/// Useful for incremental compilation.
/// Generated bytecode is in a runnable format if and only if the compiler has
/// returned to its initial state, i.e. is done. This method checks for that.
bool Compiler::is_done () {
    return state.get_state() == COMPILE_GLOBAL;
}

Compiler::Compiler (Bytecode* bytecode) : state () {
    this->lexing_string = (char*) malloc(500);
    this->bytecode = bytecode;
    this->file_counter = 0;
    this->line_counter = 0;
}

Compiler::~Compiler () {
    free (lexing_string);
}

/// Compiles code from file. Reads a text file and compiles the code inside of it.
void Compiler::compile_from_file (const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Can't open file: %s\n", filename);
        return;
    }
    
    // init the counters
    append_int(0);
    file_counter = append_string(filename);
    line_counter = 1;
    
    // append line-zero meta data
    append_instr(OPERATION_FILE);
    append_param(file_counter);
    append_instr(OPERATION_LINE);
    append_param(line_counter);
    
    // read the program code
    fseek(file, 0, SEEK_END);
    size_t file_length = ftell(file);
    fseek(file, 0, SEEK_SET);  

    char* code = (char*) malloc(file_length);   

    fread(code, 1, file_length, file);
    fclose(file);

    // compile the loaded program code
    this->compile(code);
    
    // yeet the loaded program code
    free(code);
    
    // reset the counters
    file_counter = 0;
    line_counter = 0;
}
 
}