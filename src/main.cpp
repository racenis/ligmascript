// This is a minimal runtime for the LIGMAScript version 0.1.0

// If you want to embed the LIGMAScript into your own program, then exclude
// this file when compiling the library and re-implement it in your own program.

#include <stdio.h>
#include <shared/allocator.h>
#include <shared/stack.h>

#include <compiler/compiler.h>
#include <compiler/error.h>
#include <decompiler/decompiler.h>

#include <interpreter/interpreter.h>
#include <interpreter/error.h>

#include <interpreter/coroutine.h>

using namespace ligma;


// Here's some integrations for the interpreter.

// We'll need this one so that we can stop the REPL cycle.
bool continue_interpret = true;

// After we register this function into the interpreter, we'll be able to call
// it from the LIGMAScript code.
void exit_func (RefStack* ref_stack) {
    printf("\nK THX BYE!\n");
    continue_interpret = false;
}

// This function registers an external into the interpreter.
void register_func (const char* name, void (*function) (RefStack*)) {
    
    // First we make a word with a name.
    Word* func_word = LookupWord(MakeWord(name));
    
    // Next we set its type so that the interpreter knows that its a function.
    func_word->type = WORD_EXT_FUNCTION;
    
    // Also we need to set the FLAG_EXT, so that the interpreter knows that its
    // an external word and should never be garbage collected, and also the
    // FLAG_READONLY, which will prevent us from accidentally changing the value.
    func_word->flags = FLAG_EXT | FLAG_READONLY;
    
    // Finally we give put function into the value pointer of the word. 
    func_word->external_function = function;
    
}

int main (int argc, char** argv) {
    // This function should be called before any other LIGMAScript function.
    // Also you shouldn't call it twice (your computer will literaly explode).
    InitInterpreter();
    
    // Let's register our functions into the interpreter.
    register_func("exit",       exit_func);

    // Bytecode class is a container that holds ... bytecode.
    Bytecode code1;
    Bytecode code2;
    
    // Compiler class holds the compiler state.
    Compiler compiler1 (&code1);
    Compiler compiler2 (&code2);
    
    // Decompiler class decompiles the bytecode.
    Decompiler decompiler1 (&code1);
    Decompiler decompiler2 (&code2);
    
    try {
        
        // We can compile LIGMAScript code from a file.
        compiler1.compile_from_file("library/main.ligma");
    
        // We can also stuff it directly into the compiler.
        compiler2.compile("isit (lambda if (`yes . cr) else (`no . cr)) set");
    
    } catch (CompilerError& error) {
        printf("Compile Error:\n%s\n\nOccured in fixile %s line %d.\n", 
            error.what(),
            compiler1.get_file_name(),
            compiler1.get_line_number()
        );
        
        return 0;
    }

    // After compiling we can merge bytecodes.
    code1.append(&code2);
    
    // And look at the compiled bytecode.
    decompiler1.decompile();

    // Now we can patch in our compiled bytecode into the interpreter.
    // It returns the offset of our newly-patched bytecode into the
    // interpreter's global bytecode segment.
    offset_t new_bytecode = SubmitBytecode(&code1);
    
    // This will initialize the bytecode.
    Coroutine (new_bytecode).run(1000);
    
    // This is a simple REPL loop.
    Coroutine coroutine;
    while (continue_interpret) {
        Bytecode bytecode;
        Compiler compiler (&bytecode);
        
        try {
            do {
                char buffer [500];
                gets(buffer);
                compiler.compile (buffer);
            } while (!compiler.is_done());
            
            coroutine.reset(SubmitBytecode(&bytecode));
        
            coroutine.run (400);
        } catch (InterpreterError& error) {
            printf("Runtime Error:\n%s\n\n", error.what());
            
            if (coroutine.get_file_name()) {
                printf("Occured in fixile %s line %d.\n", 
                    coroutine.get_file_name(),
                    coroutine.get_line_number()
                );
            } else {
                printf("Occured somewhere.\n");
            }
            
            continue_interpret = false;
        } catch (CompilerError& error) {
            printf("Compile Error:\n%s\n\n", 
                error.what()
            );
        }
    }

    printf("Terminating.\n");
    
    return 0;
}
