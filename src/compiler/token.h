#ifndef COMPILER_TOKEN_HclTabCtrl
#define COMPILER_TOKEN_H

namespace ligma {

enum token_t {
    TOKEN_NONE,                     // not a real token
    
    TOKEN_WORD,                     // everything else
    
    TOKEN_NIL,                      // nil
    TOKEN_TRUE,                     // true
    TOKEN_FALSE,                    // false
    TOKEN_OK,                       // ok
    TOKEN_FAIL,                     // fail
    TOKEN_MAYBE,                    // maybe
    
    TOKEN_FLOAT_LITERAL,            // number
    TOKEN_INT_LITERAL,              // number, no decimal
    TOKEN_STRING_LITERAL,           // either `literal or "literal"
    
    TOKEN_FILE,                     // filename
    TOKEN_NEWLINE,                  // newline character
    
    TOKEN_OPEN_BLOCK,               // (
    TOKEN_CLOSE_BLOCK,              // )
    
    TOKEN_OPEN_PUSH_BLOCK,          // [
    TOKEN_CLOSE_PUSH_BLOCK,         // ]
    
    TOKEN_LAMBDA_BLOCK,             // lambda
    TOKEN_VECTOR_BLOCK,             // vec
    TOKEN_LIST_BLOCK,               // list
    TOKEN_DECLARE_BLOCK,            // declare
    
    TOKEN_OPERATOR_ADD,             // +
    TOKEN_OPERATOR_SUB,             // -
    TOKEN_OPERATOR_MUL,             // *
    TOKEN_OPERATOR_DIV,             // /
    
    TOKEN_OPERATOR_MODULO,          // mod
    TOKEN_OPERATOR_FLOOR,           // floor
    
    TOKEN_OPERATOR_PRINTSTACK,      // .s
    TOKEN_OPERATOR_PRINTINFO,       // ..
    TOKEN_OPERATOR_PRINT,           // .
    TOKEN_OPERATOR_CARRIAGE_RETURN, // cr
    
    TOKEN_OPERATOR_LISTNEXT,        // next
    TOKEN_OPERATOR_LISTDATA,        // data
    TOKEN_OPERATOR_LISTNEXTSET,     // setnext
    TOKEN_OPERATOR_LISTDATASET,     // setdata
    
    TOKEN_OPERATOR_VECTORINSERT,    // <<
    TOKEN_OPERATOR_VECTOREXTRACT,   // >>
    
    TOKEN_OPERATOR_DROP,            // drop
    TOKEN_OPERATOR_OVER,            // over
    TOKEN_OPERATOR_SWAP,            // swap
    TOKEN_OPERATOR_DUPLICATE,       // dup
    TOKEN_OPERATOR_ROTATE,          // rot
    
    TOKEN_OPERATOR_EQUAL,           // ==
    TOKEN_OPERATOR_NOTEQUAL,        // !=
    TOKEN_OPERATOR_GREATERTHAN,     // >
    TOKEN_OPERATOR_LESSERTHAN,      // <
    TOKEN_OPERATOR_GREATEROREQUAL,  // >=
    TOKEN_OPERATOR_LESSEROREQUAL,   // <=
    
    TOKEN_OPERATOR_AND,             // and
    TOKEN_OPERATOR_OR,              // or
    TOKEN_OPERATOR_NOT,             // not
    
    TOKEN_OPERATOR_ASSIGN,          // set
    TOKEN_OPERATOR_LENGTH,          // len
    TOKEN_OPERATOR_EXECUTE,         // do
    TOKEN_OPERATOR_TYPE,            // type

    TOKEN_OPERATOR_IDENTITY,        // =

    TOKEN_CONTROL_IF,               // if
    TOKEN_CONTROL_ELSE,             // else
    TOKEN_CONTROL_RETURN,           // return
    TOKEN_CONTROL_REPEAT,           // repeat
    TOKEN_CONTROL_EXPLODE           // explode

};

token_t GetTokenFromString (const char* string);
token_t GetNextTokenFromString (const char* string);

}
#endif