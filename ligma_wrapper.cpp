#include <vector>
#include <fstream>
#include <thread>
#include "ligma.h"
#include "ligma_wrapper.h"

namespace ligma {
    std::vector<Bytecode> bytecodes;
    std::vector<InterpreterState> interpreters;
    ConsoleInterpreter console_interpreter;
    std::thread console_thread;

    void LoadFile (const char* filename) {
        std::ifstream input_stream (filename);
        std::string code_string ((std::istreambuf_iterator<char>(input_stream)),
                               std::istreambuf_iterator<char>());

        bytecodes.emplace_back();
        strncpy(bytecodes.back().name, filename, 14);
        bytecodes.back().compile(code_string.c_str());

        InterpreterState prep_state;
        prep_state.bytecode(&bytecodes.back());
        prep_state.interpret();
    }

    param_t ParseName (const char* name) {
        return find_word(name);
    }

    void SetValue (WordType type, size_t size, bool read_only, param_t word, void* address) {
        auto& w = all_words.get(word);
        if (address == nullptr) {
            w.clean();
        } else {
            w.size = size;
            w.value = address;
            w.no_delete = true;
            w.no_collect = true;

            switch (type) {
                case STRING:
                    w.type = Word::STRING;
                    break;
                case FLOAT32:
                    w.type = Word::FLOAT32;
                    break;
                case FLOAT64:
                    w.type = Word::FLOAT64;
                    break;
                case INT8:
                    w.type = Word::INT8;
                    break;
                case INT16:
                    w.type = Word::INT16;
                    break;
                case INT32:
                    w.type = Word::INT32;
                    break;
                case INT64:
                    w.type = Word::INT64;
                    break;
                case UINT8:
                    w.type = Word::UINT8;
                    break;
                case UINT16:
                    w.type = Word::UINT16;
                    break;
                case UINT32:
                    w.type = Word::UINT32;
                    break;
                case UINT64:
                    w.type = Word::UINT64;
                    break;
            }
        }
    }

    void Interpret (param_t word) {
        auto& w = all_words.get(word);
        if (w.type == Word::BYTECODE) {
            interpreters.emplace_back();
            interpreters.back().stack.push(InterpreterState::StackElement{(Bytecode*)w.value, (param_t)w.size});
        }
    }
    void Update () {
        for (auto it = interpreters.begin(); it < interpreters.end(); it++) {
            if (it->stack.size() > 0) it->interpret();
            if (it->stack.size() == 0) interpreters.erase(it);
        }

        collect_garbage();
    }

    void interpret_help_fn() {
        try {
            console_interpreter.interpret();
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void StartConsole () {
        console_thread = std::thread(interpret_help_fn);
        console_thread.detach();
    }
    void StopConsole () {
        console_interpreter.continue_ = false;
    }

}