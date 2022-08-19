#include "ligma.h"

const char* const ligma::bytecode_instruction_str[] = {
        "NOOP",
        "JUMP",
        "RETURN",
        "BREAK",
        "JUMPIF",
        "JUMPIFNOT",
        "RESERVED5",
        "RESERVED6",
        "RESERVED7",
        "RESERVED8",
        "RESERVED9",
        "RESERVED10",
        "RESERVED11",
        "RESERVED12",
        "LINE",
        "NATIVECALL",
        "ASSIGN",
        "EXECUTEWORD",
        "PUSHWORD",
        "VECTORIZE",
        "STACKEXECUTECOND",
        "FML4",
        "FML5",
        "FML6",
        "FML7",
        "FML8",
        "FML9",
        "FML10",
        "PRINTSTACK",
        "PRINTINFO",
        "PRINT",
        "TEST4",
        "PUSHINT32",
        "PUSHUINT32",
        "PUSHFLOAT32",
        "PUSH_LAMBDA",
        "PUSH_LIST",
        "PUSH_VECTOR",
        "PUSH_STRING",
        "LISTAPPEND",
        "LISTNEXT",
        "LISTDATA",
        "LISTNEXTASSIGN",
        "LISTDATAASSIGN",
        "VECTOREXTRACT",
        "VECTORINSERT",
        "MOVE",
        "COPY",
        "S48",
        "S49",
        "S50",
        "S51",
        "S52",
        "S53",
        "S54",
        "DROP",
        "OVER",
        "SWAP",
        "ROTATE",
        "DUPLICATE",
        "PUSHN",
        "PUSHPI",
        "PUSHE",
        "PUSHG",
        "ADDITION",
        "SUBTRACION",
        "MULTIPLICATION",
        "DIVISION",
        "MODULO",
        "EXPONENT",
        "LOGARITHM",
        "INCREMENT",
        "DECREMENT",
        "EXCREMENT",
        "INVERT",
        "ABSOLUTE",
        "SQUIRT",
        "CLAMP",
        "MIN",
        "MAX",
        "DOTPRODUCT",
        "CROSSPRODUCT",
        "NORMALIZE",
        "LENGTH",
        "DISTANCE",
        "SIN",
        "COS",
        "TAN",
        "ARCSIN",
        "ARCCOS",
        "ARCTAN",
        "RADTODEGREE",
        "DEGREETORAD",
        "S94",
        "S95",
        "S96",
        "S97",
        "S98",
        "S99",
        "EQUAL",
        "NOTEQUAL",
        "GREATERTHAN",
        "LESSERTHAN",
        "GREATEROREQUAL",
        "LESSEROREQUAL",
        "AND",
        "OR",
        "MAYBE",
        "NOT",
        "NOTXDD",
        "O",
        "FUGG",
        "BENIS"
};

const char* ligma::Exception::exceptstr() const {
    switch (error) {
        case STACK_OVERFLOW:
            return "Stack overflow";
        case STACK_UNDERFLOW:
            return "Stack underflow";
        case STACK_EMPTYACCESS:
            return "Attempting to read empty stack";
        case POOL_OVERFLOW:
            return "Pool overflow";
        case RANDOM_SQUARE_BRACKET:
            return "Random square bracket";
        case NOT_BRACKET_AFTER_IF:
            return "Expected '(' after 'if'";
        case NOT_BRACKET_AFTER_ELSE:
            return "Expected '(' after 'else'";
        case UNDEFINED_NATIVE_CALL:
            return "Undefined native call";
        case INVALID_JUMP_ADDRESS:
            return "Invalid jump address";
        case INVALID_CONDITION:
            return "Vector or undefined used as a condition";
        case LIST_APPEND_NOT_LIST:
            return "Appending to something that is not a list";
        case NOT_IMPLEMENTED:
            return "Not implemented";
        case INCOMPATIBLE_COMPARISON_TYPES:
            return "Comparison of incompatible types";
        case INCOMPATIBLE_COMPARISON_SIZES:
            return "Comparison of incompatible sizes";
        case INCOMPATIBLE_ARITHMETIC_TYPES:
            return "Arithmetic of incompatible types";
        case INCOMPATIBLE_ARITHMETIC_SIZES:
            return "Arithmetic of incompatible sizes";
        case INVALID_TYPE_CONVERSION:
            return "Invalid type conversion";
        case INVALID_TYPE_ASSIGNMENT:
            return "Invalid type assignment";
        case LIST_UNAPPENDABLE:
            return "Value not appendable to a list";
    }
}

const char* ligma::Word::type_as_str() const {
    switch (type) {
        case UNDEFINED:
            return "UNDEFINED";
        case BYTECODE:
            return "BYTECODE";
        case LIST:
            return "LIST";
        case REFERENCE:
            return "REFERENCE";
        case STRING:
            return "STRING";
        case INT64:
            return "INT64";
        case INT32:
            return "INT32";
        case INT16:
            return "INT16";
        case INT8:
            return "INT8";
        case UINT64:
            return "UINT64";
        case UINT32:
            return "UINT32";
        case UINT16:
            return "UINT16";
        case UINT8:
            return "UINT8";
        case FLOAT64:
            return "FLOAT64";
        case FLOAT32:
            return "FLOAT32";
        default:
            return "UNKNOWN TYPE";
    }
}

size_t ligma::Word::type_size() const {
    switch (type) {
        case INT64:
            return sizeof(int64_t);
        case INT32:
            return sizeof(int32_t);
        case INT16:
            return sizeof(int16_t);
        case INT8:
            return sizeof(int8_t);
        case UINT64:
            return sizeof(uint64_t);
        case UINT32:
            return sizeof(uint32_t);
        case UINT16:
            return sizeof(uint16_t);
        case UINT8:
            return sizeof(uint8_t);
        case FLOAT64:
            return sizeof(double);
        case FLOAT32:
            return sizeof(float);
        default:
            return 0;
    }
}