#include <compiler/error.h>

namespace ligma {

const char* CompilerError::what () const throw () {
    switch (this->error) {
        case COMPILER_ERROR_UNKNOWN:
            return "COMPILER_ERROR_UNKNOWN"; break;
        case COMPILER_ERROR_STATE_OVERFLOW:
            return "COMPILER_ERROR_STATE_OVERFLOW"; break;
        case COMPILER_ERROR_STATE_UNDERFLOW:
            return "COMPILER_ERROR_STATE_UNDERFLOW"; break;
        case COMPILER_ERROR_NO_BLOCK_START_AFTER_IF:
            return "COMPILER_ERROR_NO_BLOCK_START_AFTER_IF"; break;
        case COMPILER_ERROR_NO_BLOCK_START_AFTER_ELSE:
            return "COMPILER_ERROR_NO_BLOCK_START_AFTER_ELSE"; break;
        case COMPILER_ERROR_ELSE_WITHOUT_IF:
            return "COMPILER_ERROR_ELSE_WITHOUT_IF"; break;
        case IMPROPER_USE_OF_CLOSE_BLOCK:
            return "IMPROPER_USE_OF_CLOSE_BLOCK"; break;
        case IMPROPER_USE_OF_CLOSE_PUSH:
            return "IMPROPER_USE_OF_CLOSE_PUSH"; break;
        default:
            return "ERROR!";
    }
}

}
