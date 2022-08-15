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
        "TEST3",
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
        "S42",
        "S43",
        "S44",
        "S45",
        "S46",
        "S47",
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
    }
}