#include <iostream>
#include <fstream>
#include "ligma.h"
#include "ligma_wrapper.h"


int main() {
    // here's some ligmascript code
    std::string ligmascript_code = "begonis (lambda 0 call) set ";
    // we're setting the word "begonis" to a subroutine that will to a native call with the index of 0

    // a native call is a function that you can define in c++ code and then later call from your ligmascript code
    // you put pointers to them in the ligma::native_calls array
    ligma::native_calls[0] = [](ligma::InterpreterState& state){ std::cout << "Yippee!" << std::endl;};

    // ligma::Bytecode is a class that holds some bytecode
    ligma::Bytecode bytecode;

    // you can then compile our ligmascript code
    bytecode.compile(ligmascript_code.c_str());
    // and also look at the compiled bytecode
    bytecode.decompile();

    // ligma::InterpreterState holds the subroutine and word stacks
    ligma::InterpreterState interpreter_state;

    // you can push bytecode onto the subroutine stack
    interpreter_state.bytecode(&bytecode);
    // and then interpret it
    interpreter_state.interpret();

    // when we interpret our code, it will create the "begonis" word and then assign our subroutine to it
    // if we want to execute our word, we will need to find its index
    auto word_index = ligma::ParseName ("begonis");

    // then we can create an interpreter instance
    ligma::Interpret (word_index);
    // it will not be interpreted immediately, but the instance will be put in a queue
    // we can process the queue with
    ligma::Update ();
    // this will execute our subroutine, which in turn will execute our native function

    // we can also load a whole file, compile and interpret it
    ligma::LoadFile("examples/benis.ligma");

    // we can also create a console interpreter
    ligma::ConsoleInterpreter console_interpreter;
    // it will read a line from standard input, compile and interpret it
    console_interpreter.interpret();

    // you can type "exit" into the console to stop the interpreter

    return 0;
}

/*
// stuff for testing
int main() {

    ligma::LoadFile("examples/lines.ligma");

    ligma::ConsoleInterpreter console_interpreter;

    console_interpreter.interpret();

    return 0;
}
*/
