#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <exception>

namespace ligma {
    enum compiler_error_t {
        COMPILER_ERROR_UNKNOWN,                     // only for testing
        COMPILER_ERROR_STATE_OVERFLOW,              // pushed too many states onto the compiler state stack
        COMPILER_ERROR_STATE_UNDERFLOW,             // popped too many states off of the compiler state stack
        COMPILER_ERROR_NO_BLOCK_START_AFTER_IF,     // no '(' after an 'if'
        COMPILER_ERROR_NO_BLOCK_START_AFTER_ELSE,   // no '(' after an 'else'
        COMPILER_ERROR_ELSE_WITHOUT_IF,             // 'else ([...]' without 'if ([...])' first
        IMPROPER_USE_OF_CLOSE_BLOCK,                // stray ')'
        IMPROPER_USE_OF_CLOSE_PUSH                  // stray ']'
        
    };
    
    struct CompilerError : public std::exception {
        CompilerError(compiler_error_t error) {
            this->error = error;
        }
        
        const char* what () const throw ();
        
        compiler_error_t error;
    };
}

#endif