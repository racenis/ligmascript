#include <interpreter/interpreter.h>
#include <interpreter/coroutine.h>
#include <interpreter/error.h>
#include <shared/instructions.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <map>
#include <string>

namespace ligma {
enum Atoms {
    ATOM_NIL,
    ATOM_TRUE,
    ATOM_FALSE,
    ATOM_OK,
    ATOM_FAIL,
    ATOM_MAYBE,
    
    ATOM_ATOM,
    ATOM_INT,
    ATOM_FLOAT,
    ATOM_STRING,
    ATOM_BYTECODE,
    ATOM_LIST,
    ATOM_INT8,
    ATOM_INT16,
    ATOM_INT32,
    ATOM_INT64,
    ATOM_FLOAT32,
    ATOM_FLOAT64,
    ATOM_CSTRING,
    ATOM_FUNCTION,
    
    ATOM_LAST
};
  
const size_t RUNTIME_WORD_ARRAY_SIZE = 60000;
  
char* RUNTIME_BYTECODE = NULL;
char* RUNTIME_LITERALS = NULL;
Word* RUNTIME_WORDS = NULL;
Word** RUNTIME_WORD_FREE_LIST = NULL;


char* RUNTIME_BYTECODE_END = NULL;
char* RUNTIME_LITERALS_END = NULL;
Word* RUNTIME_WORDS_END = NULL;
Word** RUNTIME_WORD_FREE_LIST_END = NULL;

Bytecode RUNTIME_BYTECODE_OBJ;

bool RUNTIME_INITED = false;

std::map<atom_t, std::string> ATOM_MAP;
std::map<std::string, param_t> WORD_MAP;

/// Initializes the interpreter.
/// Allocates memory for the interpreter internals and does some more initialization stuff.
/// @warning If you call this function twice, your computer will literally explode.
void InitInterpreter() {
    if (RUNTIME_INITED) {
        throw InterpreterError (INTERPRETER_ERROR_UNKNOWN); // TODO: make this a proper error
    }
    
    RUNTIME_WORDS = RUNTIME_WORDS_END = (Word*) malloc(RUNTIME_WORD_ARRAY_SIZE * sizeof(Word));
    RUNTIME_WORD_FREE_LIST = (Word**) malloc(RUNTIME_WORD_ARRAY_SIZE * sizeof(void*));
    RUNTIME_WORD_FREE_LIST_END = RUNTIME_WORD_FREE_LIST;
    
    memset(RUNTIME_WORDS, 0, RUNTIME_WORD_ARRAY_SIZE * sizeof(Word));
    memset(RUNTIME_WORD_FREE_LIST, 0, RUNTIME_WORD_ARRAY_SIZE * sizeof(void*));
    
    for (atom_t i = 0; i < ATOM_LAST; i++) {
        LookupWord(MakeWord())->atom_value = i;
        LookupWord(i)->flags = FLAG_SYMBOL | FLAG_READONLY;
    }
    
    ATOM_MAP[ATOM_NIL] = "nil";
    ATOM_MAP[ATOM_TRUE] = "true";
    ATOM_MAP[ATOM_FALSE] = "false";
    ATOM_MAP[ATOM_OK] = "ok";
    ATOM_MAP[ATOM_FAIL] = "fail";
    ATOM_MAP[ATOM_MAYBE] = "maybe";
    
    ATOM_MAP[ATOM_ATOM] = "type:atom";
    ATOM_MAP[ATOM_INT] = "type:int";
    ATOM_MAP[ATOM_FLOAT] = "type:float";
    ATOM_MAP[ATOM_STRING] = "type:string";
    ATOM_MAP[ATOM_BYTECODE] = "type:lambda";
    ATOM_MAP[ATOM_LIST] = "type:list";
    ATOM_MAP[ATOM_INT8] = "type:int8";
    ATOM_MAP[ATOM_INT16] = "type:int16";
    ATOM_MAP[ATOM_INT32] = "type:int32";
    ATOM_MAP[ATOM_INT64] = "type:int64";
    ATOM_MAP[ATOM_FLOAT32] = "type:float32";
    ATOM_MAP[ATOM_FLOAT64] = "type:float64";
    ATOM_MAP[ATOM_CSTRING] = "type:cstring";
    ATOM_MAP[ATOM_FUNCTION] = "type:function";
    
    WORD_MAP["type:atom"] = ATOM_ATOM;
    WORD_MAP["type:int"] = ATOM_INT;
    WORD_MAP["type:float"] = ATOM_FLOAT;
    WORD_MAP["type:string"] = ATOM_STRING;
    WORD_MAP["type:lambda"] = ATOM_BYTECODE;
    WORD_MAP["type:list"] = ATOM_LIST;
    WORD_MAP["type:int8"] = ATOM_INT8;
    WORD_MAP["type:int16"] = ATOM_INT16;
    WORD_MAP["type:int32"] = ATOM_INT32;
    WORD_MAP["type:int64"] = ATOM_INT64;
    WORD_MAP["type:float32"] = ATOM_FLOAT32;
    WORD_MAP["type:float64"] = ATOM_FLOAT64;
    WORD_MAP["type:cstring"] = ATOM_CSTRING;
    WORD_MAP["type:function"] = ATOM_FUNCTION;
    
    RUNTIME_INITED = true;
}

/// Patches a bytecode into the global interpreter bytecode.
/// Patches all of the segments from new_bytecode into the global interpeter
/// bytecode segments and links all of the symbols.
/// @return Start position of the patched bytecode. You can put this value into
///         the Coroutine::run() method.
offset_t SubmitBytecode (Bytecode* new_bytecode) {
    if (!RUNTIME_INITED) {
        throw InterpreterError (INTERPRETER_ERROR_UNKNOWN); // TODO: make this a proper error
    }
    
    offset_t new_bytecode_start = RUNTIME_BYTECODE_OBJ.bytecode.size();
    offset_t new_symbols_start =  RUNTIME_BYTECODE_OBJ.symbols.size();
    
    // appending the new bytecode to the already loaded bytecode
    RUNTIME_BYTECODE_OBJ.append(new_bytecode);
    
    // appending a return instruction for the initialization 
    RUNTIME_BYTECODE_OBJ.bytecode.append(OPERATION_RETURN);
    
    // patching in the new symbols
    char* new_symbols = RUNTIME_BYTECODE_OBJ.symbols.begin() + new_symbols_start;
    char* new_symbols_end = RUNTIME_BYTECODE_OBJ.symbols.end();
    
    while (new_symbols < new_symbols_end) {
        offset_t* symbol_name = (offset_t*) new_symbols;
        offset_t* symbol_offset = (offset_t*) (new_symbols + sizeof(offset_t));
        
        char* word_name = RUNTIME_BYTECODE_OBJ.literals.begin() + *symbol_name;
        char* word_offset = RUNTIME_BYTECODE_OBJ.bytecode.begin() + *symbol_offset;
        
        param_t new_word = MakeWord(word_name);
        *((param_t*) word_offset) = new_word;
        
        new_symbols += sizeof(offset_t) * 2;
    }
    
    // updating interpreter bytecode sections
    RUNTIME_BYTECODE = RUNTIME_BYTECODE_OBJ.bytecode.begin();
    RUNTIME_LITERALS = RUNTIME_BYTECODE_OBJ.literals.begin();
    
    return new_bytecode_start;
}

/// Makes a new word.
/// @return Reference to the newly created word.
param_t MakeWord () {
    Word* new_word = NULL;
    
    if (RUNTIME_WORD_FREE_LIST == RUNTIME_WORD_FREE_LIST_END) {
        new_word = RUNTIME_WORDS_END++;
    } else {
        new_word = *(--RUNTIME_WORD_FREE_LIST_END);
    }
    
    memset (new_word, 0, sizeof(Word));
    return new_word - RUNTIME_WORDS;
}

/// Makes a word from a symbol.
/// If a word with the same symbol has already been created, then it won't be
/// created again, and instead a reference to the existing word will be returned.
/// @return Reference to a word with the symbol.
param_t MakeWord (const char* symbol) {
    param_t word = WORD_MAP[symbol];
    
    if (!word) {
        static atom_t atom_counter = 100;
        word = MakeWord();
        Word* wordp = LookupWord(word);
        wordp->atom_value = atom_counter;
        wordp->flags |= FLAG_SYMBOL;
        WORD_MAP[symbol] = word;
        ATOM_MAP[atom_counter] = symbol;
        atom_counter++;
    }
    
    return word;
}

/// Finds the name of the atom.
/// @return A null-terminated string.
const char* LookupAtom (atom_t atom) {
    return ATOM_MAP[atom].c_str();
}

/// Converts a word reference to a word pointer.
Word* LookupWord (param_t id) {
    return RUNTIME_WORDS + id;
}

/// Checks if a word can be garbage collected.
/// @warning This function can possibly destroy the word and free its value, 
///          so call it if and only if you will never access the word again.
void CleanWord (Word* word) {
    if (word->flags & FLAG_EXT || 
        word->flags & FLAG_READONLY || 
        word->flags & FLAG_SYMBOL ||
        word->refs > 0) {
        return;
    }
    
    word->clear();
    *(RUNTIME_WORD_FREE_LIST_END++) = word;
}

/// Finds the file name of currently executing code.
/// Finds the name of the file from which the currently executing code has been
/// compiled. This only works if the code was compiled with the Compiler class
/// compile_from_file() method.
/// @return A null-terminated string to the file name or NULL if there is no file.
const char* Coroutine::get_file_name() {
    if (!file_counter) {
        return NULL;
    }
    
    return RUNTIME_LITERALS + file_counter;
}

/// Finds the line number of currently executing code.
/// Finds the name of the file from which the currently executing code has been
/// compiled. This only works if the code has the appropriate meta-data.
/// @return Line number or 0 if there is no file.
offset_t Coroutine::get_line_number() {
    return line_counter;
}

/// Runs a coroutine.
/// Runs a coroutine for a number of specified instructions.
void Coroutine::run (size_t iterations) {
for (size_t i = 0; i < iterations; i++) {
    instr_t instruction = *(RUNTIME_BYTECODE + program_counter);
    param_t parameter = *((param_t*)(RUNTIME_BYTECODE + program_counter + 1));
    
    switch (instruction) {
        case OPERATION_NOOP:
            break;
        case OPERATION_JUMP:
            this->program_counter = parameter;
            continue;
        case OPERATION_JUMP_IF_NOT: {
            Word* word = LookupWord(ref_stack.pop());
            
            if (word->type != WORD_ATOM) {
                assert(false && "THROW ERROR, JUMP IF NOT ATOM");
            }
            
            if (word->atom_value == ATOM_FALSE) {
                this->program_counter = parameter;
                CleanWord (word);
                continue;
            }
            
            CleanWord (word);
            } break;
        case OPERATION_ASSIGN: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());

            if (p1->flags & FLAG_READONLY) {
                throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_READ_ONLY);
            }

            if (p1->is_external()) {
                if (p1->is_int()) {
                    if (!p2->is_numeric()) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_NON_NUMERIC_TO_EXT_INT);
                    }
                    
                    if (p1->length != p2->length) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_TO_EXT_LENGTH_MISMATCH);
                    }
                    
                    int_t* val = p2->value_as_int();
                    p1->deposit_int(val);
                    if (p2->type != WORD_INT) free (val);
                } else if (p1->is_float()) {
                    if (!p2->is_numeric()) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_NON_NUMERIC_TO_EXT_FLOAT);
                    }
                    
                    if (p1->length != p2->length) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_TO_EXT_LENGTH_MISMATCH);
                    }
                    
                    float_t* val = p2->value_as_float();
                    p1->deposit_float(val);
                    if (p2->type != WORD_FLOAT) free (val);
                } else if (p1->is_string()) {
                    if (!p2->is_string()) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_NON_STRING_TO_EXT_STRING);
                    }
                    
                    if (p1->length < p2->length) {
                        throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_TO_EXT_STRING_NO_SPACE);
                    }
                    
                    char* val = p2->value_as_string();
                    p1->deposit_string(val);
                    if (p2->type != WORD_STRING) free (val);
                } else {
                    throw InterpreterError(INTERPRETER_ERROR_ASSIGNING_TO_ILLEGAL_EXT);
                }
            } else {
                p1->clear();
                p1->make_copy(p2);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_ALLOCATE_LOCAL:
            lcl_stack.push_locals(parameter);
            break;
        case OPERATION_DEALLOCATE_LOCAL:
            lcl_stack.pop_locals(parameter);
            break;
        case OPERATION_FILE:
            file_counter = parameter;
            break;
        case OPERATION_LINE:
            line_counter = parameter;
            break;
        case OPERATION_LISTNEXT: {
            Word* word = LookupWord(ref_stack.pop());
            
            if (word->type != WORD_LIST) {
                throw InterpreterError(INTERPRETER_ERROR_LIST_OPERATION_ON_NON_LIST);
            }
            
            ref_stack.push(word->list_value.next);
            CleanWord (word);
            } break;
        case OPERATION_LISTDATA: {
            Word* word = LookupWord(ref_stack.pop());
            
            if (word->type != WORD_LIST) {
                throw InterpreterError(INTERPRETER_ERROR_LIST_OPERATION_ON_NON_LIST);
            }
            
            ref_stack.push(word->list_value.data);
            CleanWord (word);
            } break;
        case OPERATION_LISTNEXTSET: {
            param_t ref = ref_stack.pop();
            Word* next = LookupWord(ref);
            Word* list = LookupWord(ref_stack.pop());
            
            if (list->type != WORD_LIST) {
                throw InterpreterError(INTERPRETER_ERROR_LIST_OPERATION_ON_NON_LIST);
            }
            
            if (list->list_value.next) {
                LookupWord(list->list_value.next)->refs--;
            }
            
            next->refs++;
            
            list->list_value.next = ref;
            
            CleanWord (next);
            CleanWord (list);
            } break;
        case OPERATION_LISTDATASET: {
            param_t ref = ref_stack.pop();
            Word* data = LookupWord(ref);
            Word* list = LookupWord(ref_stack.pop());
            
            if (list->type != WORD_LIST) {
                throw InterpreterError(INTERPRETER_ERROR_LIST_OPERATION_ON_NON_LIST);
            }
            
            if (list->list_value.data) {
                LookupWord(list->list_value.data)->refs--;
            }
            
            data->refs++;
            
            list->list_value.data = ref;
            
            CleanWord (data);
            CleanWord (list);
            } break;
        case OPERATION_DROP: {
            Word* word = LookupWord(ref_stack.pop());
            CleanWord (word);
            } break;
        case OPERATION_OVER: {
            param_t first = ref_stack.pop();
            param_t second = ref_stack.pop();
            ref_stack.push(second);
            ref_stack.push(first);
            ref_stack.push(second);
            } break;
        case OPERATION_SWAP: {
            param_t first = ref_stack.pop();
            param_t second = ref_stack.pop();
            ref_stack.push(first);
            ref_stack.push(second);
            } break;
        case OPERATION_DUPLICATE: {
            param_t top = ref_stack.pop();
            ref_stack.push(top);
            ref_stack.push(top);
            } break;
        case OPERATION_ROTATE: {
            param_t first = ref_stack.pop();
            param_t second = ref_stack.pop();
            param_t third = ref_stack.pop();
            ref_stack.push(second);
            ref_stack.push(first);
            ref_stack.push(third);
            } break;
        case OPERATION_EQUAL: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_equal_to(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_NOTEQUAL: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_not_equal_to(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_GREATERTHAN: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_greater_than(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_LESSERTHAN: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_lesser_than(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_GREATEROREQUAL: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_greater_or_equal_to(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_LESSEROREQUAL: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->is_lesser_or_equal_to(p2)) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_AND: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());

            if (p1->type != WORD_ATOM || p1->type != WORD_ATOM) {
                assert(false && "THROW ERROR, JUMP IF NOT ATOM");
            }
            
            if (p1->atom_value == ATOM_TRUE && p2->atom_value == ATOM_TRUE) {
                ref_stack.push(ATOM_TRUE);
            } else if ((p1->atom_value != ATOM_FALSE && p2->atom_value != ATOM_FALSE)) {
                ref_stack.push(ATOM_MAYBE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_OR: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->type != WORD_ATOM || p1->type != WORD_ATOM) {
                assert(false && "THROW ERROR, JUMP IF NOT ATOM");
            }
            
            if (p1->atom_value == ATOM_TRUE || p2->atom_value == ATOM_TRUE) {
                ref_stack.push(ATOM_TRUE);
            } else if (p1->atom_value == ATOM_MAYBE || p2->atom_value == ATOM_MAYBE) {
                ref_stack.push(ATOM_MAYBE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_NOT: {
            Word* p1 = LookupWord(ref_stack.pop());
            
            if (p1->type != WORD_ATOM) {
                assert(false && "THROW ERROR, JUMP IF NOT ATOM");
            }
            
            if (p1->atom_value == ATOM_TRUE) {
                ref_stack.push(ATOM_FALSE);
            } else if (p1->atom_value == ATOM_FALSE) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_MAYBE);
            }
            
            CleanWord (p1);
            } break;
        case OPERATION_IDENTITY: {
            Word* p1 = LookupWord(ref_stack.pop());
            Word* p2 = LookupWord(ref_stack.pop());
            
            if (p1 == p2) {
                ref_stack.push(ATOM_TRUE);
            } else {
                ref_stack.push(ATOM_FALSE);
            }
            
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_LENGTH: {
            Word* mesure = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* word = LookupWord(ref);
            
            word->flags = FLAG_TEMP;
            word->length = 1;
            word->type = WORD_INT;
            word->int_value = (int_t*) malloc (sizeof(int_t));
            *word->int_value = mesure->length;
            
            ref_stack.push(ref);
            CleanWord (mesure);
            } break;
        case OPERATION_TYPE: {
            Word* mesure = LookupWord(ref_stack.pop());

            switch (mesure->type) {
                case WORD_ATOM: ref_stack.push(ATOM_ATOM); break;
                case WORD_INT: ref_stack.push(ATOM_INT); break;
                case WORD_FLOAT: ref_stack.push(ATOM_FLOAT); break;
                case WORD_STRING: ref_stack.push(ATOM_STRING); break;
                case WORD_BYTECODE: ref_stack.push(ATOM_BYTECODE); break;
                case WORD_LIST: ref_stack.push(ATOM_LIST); break;

                case WORD_EXT_INT8: ref_stack.push(ATOM_INT8); break;
                case WORD_EXT_INT16: ref_stack.push(ATOM_INT16); break;
                case WORD_EXT_INT32: ref_stack.push(ATOM_INT32); break;
                case WORD_EXT_INT64: ref_stack.push(ATOM_INT64); break;
                case WORD_EXT_FLOAT32: ref_stack.push(ATOM_FLOAT32); break;
                case WORD_EXT_FLOAT64: ref_stack.push(ATOM_FLOAT64); break;
                case WORD_EXT_STRING: ref_stack.push(ATOM_CSTRING); break;
                case WORD_EXT_FUNCTION: ref_stack.push(ATOM_FUNCTION); break;
            }
            
            CleanWord (mesure);
            } break;
        case OPERATION_EXECUTE_WORD: {
            param_t ref = ref_stack.pop();
            Word* word = LookupWord(ref);
            
            switch (word->type) {
                case WORD_EXT_FUNCTION:
                    word->external_function(&ref_stack);
                    break;
                case WORD_BYTECODE: {
                    JmpStackValues jump_values;
                    
                    jump_values.offset = program_counter + BytecodeInstructionLength(instruction);
                    jump_values.file = file_counter;
                    jump_values.line = line_counter;
                    
                    jmp_stack.push(jump_values);
                    
                    program_counter = word->bytecode_value;
                    } break;
                default:
                    ref_stack.push(ref);
                    break;
            }
            
            CleanWord (word);
            } break;
        case OPERATION_PUSH_FLOAT: {
            param_t ref = MakeWord();
            Word* word = LookupWord(ref);
            
            word->flags = FLAG_TEMP;
            word->length = 1;
            word->type = WORD_FLOAT;
            word->float_value = (float_t*) malloc (sizeof(float_t));
            *word->float_value = *(float_t*)(RUNTIME_LITERALS + parameter);
            
            ref_stack.push(ref);
            } break;
        case OPERATION_PUSH_INT: {
            param_t ref = MakeWord();
            Word* word = LookupWord(ref);
            
            word->flags = FLAG_TEMP;
            word->length = 1;
            word->type = WORD_INT;
            word->int_value = (int_t*) malloc (sizeof(int_t));
            *word->int_value = *(int_t*)(RUNTIME_LITERALS + parameter);
            
            ref_stack.push(ref);
            } break;
        case OPERATION_PUSH_STRING: {
            param_t ref = MakeWord();
            Word* word = LookupWord(ref);
            
            char* val = (char*)(RUNTIME_LITERALS + parameter);
            
            word->flags = FLAG_TEMP | FLAG_LITERAL;
            word->length = strlen(val);
            word->type = WORD_STRING;
            word->string_value = val;
            
            ref_stack.push(ref);
            } break;
        case OPERATION_PUSH_WORD:
            ref_stack.push(parameter);
            break;
        case OPERATION_PUSH_LOCAL:
            ref_stack.push(lcl_stack.get_local(parameter));
            break;
        case OPERATION_PUSH_LAMBDA: {
            param_t ref = MakeWord();
            Word* word = LookupWord(ref);
            
            word->flags = FLAG_TEMP;
            word->length = 1;
            word->type = WORD_BYTECODE;
            word->bytecode_value = parameter;
            
            ref_stack.push(ref);
            } break;
        case OPERATION_MAKE_VECTOR: {
            if (parameter == 0) {
                throw InterpreterError(INTERPRETER_ERROR_ILLEGAL_VECTOR_LENGTH);
            }
            
            param_t ref = MakeWord();
            Word* vec = LookupWord(ref);
            
            Word* p = LookupWord(ref_stack.pop());
            if (!p->is_numeric()) {
                throw InterpreterError(INTEPRETER_ERROR_VECTOR_MAKE_NON_NUMERIC);
            }
                
            vec->type = p->is_int() ? WORD_INT : WORD_FLOAT;
            vec->length = parameter;
            vec->flags = FLAG_TEMP;
            
            if (vec->type == WORD_INT) {
                vec->int_value = (int_t*) malloc (sizeof(int_t) * vec->length);
                
                for (param_t i = vec->length; i != 0; i--) {
                    if (p->length > 1) {
                        throw InterpreterError(INTERPRETER_ERROR_MAKE_VECTOR_IN_VECTOR);
                    } else if (!p->is_int()) {
                        throw InterpreterError(INTERPRETER_ERROR_MAKE_INT_VECTOR_NON_INT);
                    }
                    int_t* val = p->value_as_int();
                    vec->int_value[i-1] = *val;
                    if (p->type != WORD_INT) free (val);
                    CleanWord (p);
                    if (i > 1) p = LookupWord(ref_stack.pop());
                }
            } else {
                vec->float_value = (float_t*) malloc (sizeof(float_t) * vec->length);
                
                for (param_t i = vec->length; i != 0; i--) {
                    if (p->length > 1) {
                        throw InterpreterError(INTERPRETER_ERROR_MAKE_VECTOR_IN_VECTOR);
                    } else if (!p->is_float()) {
                        throw InterpreterError(INTERPRETER_ERROR_MAKE_INT_VECTOR_NON_INT);
                    }
                    float_t* val = p->value_as_float();
                    vec->float_value[i-1] = *val;
                    if (p->type != WORD_FLOAT) free (val);
                    CleanWord (p);
                    if (i > 1) p = LookupWord(ref_stack.pop());
                }
            }
            
            ref_stack.push(ref);
        } break;
        case OPERATION_VECTOREXTRACT: {
            Word* index = LookupWord(ref_stack.pop());
            Word* vec = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            
            if (!index->is_numeric()) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_EXTRACT_NON_NUMERIC_INDEX);
            }
            
            if (index->length != 1) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_EXTRACT_VECTOR_INDEX);
            }
            
            if (!vec->is_numeric() && !vec->is_string()) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_EXTRACT_ILLEGAL_VECTOR);
            }
            
            res->type = vec->is_int() ? WORD_INT : vec->is_float() ? WORD_FLOAT : WORD_STRING;
            res->length = 1;
            
            int_t* index_val = index->value_as_int();
            
            if (*index_val >= vec->length) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_EXTRACT_OUT_OF_BOUNDS);
            }
            
            if (vec->is_int()) {
                int_t* val = vec->value_as_int();
                res->int_value = (int_t*) malloc (sizeof(int_t));
                *res->int_value = val[*index_val];
                if (vec->type != WORD_INT) free (val);
            } else if (vec->is_float()) {
                float_t* val = vec->value_as_float();
                res->float_value = (float_t*) malloc (sizeof(float_t));
                *res->float_value = val[*index_val];
                if (vec->type != WORD_FLOAT) free (val);
            } else {
                char_t* val = vec->value_as_string();
                res->string_value = (char_t*) malloc (2 * sizeof(char_t));
                res->string_value[0] = val[*index_val];
                res->string_value[1] = '\0';
                if (vec->type != WORD_STRING) free (val);
            }
            
            if (index->type != WORD_INT) free (index_val);
            ref_stack.push(ref);
            
            CleanWord (index);
            CleanWord (vec);
        } break;
        case OPERATION_VECTORINSERT: {
            Word* index = LookupWord(ref_stack.pop());
            Word* value = LookupWord(ref_stack.pop());
            Word* vec = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            
            if (!index->is_numeric()) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_NON_NUMERIC_INDEX);
            }
            
            if (index->length != 1) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_VECTOR_INDEX);
            }
            
            if (!vec->is_numeric() && !vec->is_string()) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_ILLEGAL_VECTOR);
            }
            
            if ((vec->is_numeric() && !value->is_numeric()) || (vec->is_string() && !value->is_string())) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_ILLEGAL_VALUE);
            }
            
            if (value->length != 1) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_VECTOR_IN_VECTOR);
            }
            
            res->make_copy(vec);
            
            int_t* index_val = index->value_as_int();
            
            if (*index_val >= vec->length) {
                throw InterpreterError(INTERPRETER_ERROR_VECTOR_INSERT_OUT_OF_BOUNDS);
            }
            
            if (res->is_int()) {
                int_t* val = value->value_as_int();
                res->int_value[*index_val] = *val;
                if (value->type != WORD_INT) free (val);
            } else if (res->is_float()) {
                float_t* val = value->value_as_float();
                res->float_value[*index_val] = *val;
                if (value->type != WORD_FLOAT) free (val);
            } else {
                char_t* val = value->value_as_string();
                res->string_value[*index_val] = *val;
                if (value->type != WORD_STRING) free (val);
            }
            
            if (index->type != WORD_INT) free (index_val);
            ref_stack.push(ref);
            
            CleanWord (index);
            CleanWord (value);
            CleanWord (vec);
            } break;
        case OPERATION_MAKE_LIST: {
            param_t prev_word = 0;
            for (param_t i = 0; i < parameter; i++) {
                param_t new_word = MakeWord();
                Word* list_elem = LookupWord(new_word);
                
                list_elem->type = WORD_LIST;
                list_elem->length = 1;
                list_elem->list_value.next = prev_word;
                list_elem->list_value.data = ref_stack.pop();
                LookupWord(list_elem->list_value.next)->refs++;
                LookupWord(list_elem->list_value.data)->refs++;
                
                prev_word = new_word;
            }
            
            if (parameter > 0) {    // if we made a list with some stuff in it
                ref_stack.push(prev_word);
            } else {                // otherwise, make a blank list
                param_t new_word = MakeWord();
                Word* list_elem = LookupWord(new_word);
                
                list_elem->type = WORD_LIST;
                list_elem->length = 1;
                list_elem->list_value.next = 0;
                list_elem->list_value.data = 0;
                
                ref_stack.push(new_word);
            }
            } break;
        case OPERATION_ADD: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            res->make_result_addition(p1, p2);
            ref_stack.push(ref);
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_SUB: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            res->make_result_subtraction(p1, p2);
            ref_stack.push(ref);
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_MUL: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            res->make_result_multiplication(p1, p2);
            ref_stack.push(ref);
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_DIV: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            res->make_result_division(p1, p2);
            ref_stack.push(ref);
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_MOD: {
            Word* p2 = LookupWord(ref_stack.pop());
            Word* p1 = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* res = LookupWord(ref);
            res->make_result_modulo(p1, p2);
            ref_stack.push(ref);
            CleanWord (p1);
            CleanWord (p2);
            } break;
        case OPERATION_FLOOR: {
            Word* word = LookupWord(ref_stack.pop());
            param_t ref = MakeWord();
            Word* floor = LookupWord(ref);
            
            if (!word->is_numeric()) {
                // TODO: error
                printf("ERROER!");
            } else if (word->type == WORD_INT) {
                floor->make_copy(word);
            } else {
                floor->flags = FLAG_TEMP;
                floor->length = word->length;
                floor->type = WORD_INT;
                floor->int_value = word->value_as_int();
            }
            
            ref_stack.push(ref);
            CleanWord (word);
            } break;
        case OPERATION_PRINT: {
            // maybe should make this a method on the word
            Word* word = LookupWord(ref_stack.pop());
            char* str = word->value_as_string();
            printf("%s", str);
            if (word->type != WORD_STRING) free(str);
            CleanWord (word);
            } break;
        case OPERATION_PRINTSTACK:
            ref_stack.print();
            break;
        case OPERATION_CARRIAGE_RETURN:
            printf("\n");
            break;
        case OPERATION_EXPLODE:
            throw InterpreterError (INTERPRETER_ERROR_EXPLODE);
            break;
        case OPERATION_RETURN: {
            if (jmp_stack.is_empty()) {
                return;
            }
            
            JmpStackValues jump_values = jmp_stack.pop();
            
            program_counter = jump_values.offset;
            file_counter = jump_values.file;
            line_counter = jump_values.line;
            } continue; // !!
        default:
            printf("Instruction unrecognized %s\n", BytecodeInstructionShortName(instruction));
    }
    
    program_counter += BytecodeInstructionLength(instruction);
}

printf("Ran out of loops!\n");

}

}
