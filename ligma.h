#ifndef LIGMA_H
#define LIGMA_H

#include <cstdint>
#include <iostream>
#include <cstring>

namespace ligma {
    typedef uint8_t instr_t;    // bytecode instruction
    typedef uint16_t param_t;   // bytecode instruction parameter

    // TODO: add some safety check asserts for the templates
    template <typename T, size_t maxsize>
    class Stack {
        T stack[maxsize];
        size_t last = 0;

    public:
        void push (const T& element) {
            stack[last] = element;
            last++;

            if (last > maxsize) {
                std::cout << "Stack size exceeded. Aborting." << std::endl;
                abort();
            }
        }

        T& pop () {
            if (last == 0) {
                std::cout << "Attempting to pop an empty stack. Aborting." << std::endl;
                abort();
            }
            last--;
            return stack[last];
        }

        T& top () {
            if (last == 0) {
                std::cout << "Attempting to access an empty stack. Aborting." << std::endl;
                abort();
            }
            return stack[last - 1];
        }

        size_t size() { return last; }
    };

    template <typename T, size_t maxsize>
    class Pool {
        T pool[maxsize];
        size_t lastfree = -1ull;
        size_t last = 0;

    public:
        param_t add (const T& element) {
            size_t selected_space;

            if (lastfree != -1ull) {
                selected_space = lastfree;
                lastfree = *(((uint64_t*)(&pool[lastfree])) + 1);
            } else {
                selected_space = last;
                last++;
            }

            pool[selected_space] = element;
            if (last > maxsize) {
                std::cout << "Stack size exceeded. Aborting." << std::endl;
                abort();
            }

            return selected_space;
        }

        void remove (const param_t ref) {
            *((uint64_t*)(&pool[ref])) = 0;
            *((uint64_t*)(&pool[ref]) + 1) = lastfree;
            lastfree = ref;
        }

        T& get (const param_t ref) {
            return pool[ref];
        }

        size_t size () { return last; }

        bool is_valid (const param_t ref) { return *((uint64_t*)(&pool[ref])) != 0; }
    };

    enum bytecode_instruction : instr_t {
        NOOP,
        JUMP,
        RETURN,
        BREAK,
        JUMPIF,
        JUMPIFNOT,
        RESERVED5,
        RESERVED6,
        RESERVED7,
        RESERVED8,
        RESERVED9,
        RESERVED10,
        RESERVED11,
        RESERVED12,
        RESERVED13,
        NATIVECALL,
        ASSIGN,
        EXECUTEWORD,
        PUSHWORD,
        VECTORIZE,
        STACKEXECUTECOND,
        FML4,
        FML5,
        FML6,
        FML7,
        FML8,
        FML9,
        FML10,
        PRINTSTACK,
        PRINTINFO,
        TEST3,
        TEST4,
        PUSHINT64,
        PUSHUINT64,
        PUSHFLOAT32,
        PUSH_LAMBDA,
        PUSH_LIST,
        PUSH_VECTOR,
        PUSH_STRING,
        LISTAPPEND,
        S40,
        S41,
        S42,
        S43,
        S44,
        S45,
        S46,
        S47,
        S48,
        S49,
        S50,
        S51,
        S52,
        S53,
        S54,
        DROP,
        OVER,
        SWAP,
        ROTATE,
        DUPLICATE,
        PUSHN,
        PUSHPI,
        PUSHE,
        PUSHG,
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        MODULO,
        EXPONENT,
        LOGARITHM,
        INCREMENT,
        DECREMENT,
        EXCREMENT,
        INVERT,
        ABSOLUTE,
        SQUIRT,
        CLAMP,
        MIN,
        MAX,
        DOTPRODUCT,
        CROSSPRODUCT,
        NORMALIZE,
        LENGTH,
        DISTANCE,
        SIN,
        COS,
        TAN,
        ARCSIN,
        ARCCOS,
        ARCTAN,
        RADTODEGREE,
        DEGREETORAD,
        S94,
        S95,
        S96,
        S97,
        S98,
        S99,
        EQUAL,
        NOTEQUAL,
        GREATERTHAN,
        LESSERTHAN,
        GREATEROREQUAL,
        LESSEROREQUAL,
        AND,
        OR,
        MAYBE,
        NOT,
        NOTXDD,
        O,
        FUGG,
        BENIS
    };

    extern const char* const bytecode_instruction_str[];

    // TODO: make exceptions more meaningful
    class Exception : std::exception {
        const std::string exception_str;
        public:
        Exception(const std::string exception_str) : exception_str(exception_str) {}
        const char* what(){ return exception_str.c_str(); }
    };

    void collect_garbage ();
    param_t find_word (const char* name);

    struct Word {
        enum Type : uint64_t {
            UNDEFINED,
            BYTECODE,
            LIST,
            STRING,
            INT64,
            INT32,
            INT16,
            INT8,
            UINT64,
            UINT32,
            UINT16,
            UINT8,
            FLOAT64,
            FLOAT32
        };

        Type type = UNDEFINED;
        void* value = nullptr;
        size_t size = 0;
        size_t references = 0;
        bool no_delete = true;
        bool no_collect = true;

        // TODO: add a seperate print and printinfo method
        void print_info();

        const char *type_as_str() const;

        size_t type_size() const;

        bool type_is_int () { return type == INT64 || type == INT32 || type == INT16 || type == INT8;}
        bool type_is_uint () { return type == UINT64 || type == UINT32 || type == UINT16 || type == UINT8;}
        bool type_is_float () { return type == FLOAT64 || type == FLOAT32;}
        bool type_is_numeric () { return type_is_int() || type_is_uint() || type_is_float(); }

        int64_t value_as_int (size_t vec_index); uint64_t value_as_uint (size_t vec_index); float value_as_float (size_t vec_index);

        void clean ();
        void copy_from (Word& other);

    };

    /// pool where all the words are stored
    extern Pool<Word, 500> all_words;

    struct List {
        struct ListElement {
            Word* word = nullptr;
            ListElement* next = nullptr;
        };

        ListElement* first = nullptr;
        ListElement* last = nullptr;

        void append (param_t ref) {
            auto& word = all_words.get(ref);
            auto* element = new ListElement { &word };
            word.references++;

            if (last) {
                last->next = element;
                last = element;
            } else {
                first = element;
                last = element;
            }
        }

        void clean () {
            auto* elem_f = first;

            while (elem_f) {
                auto* elem_n = elem_f->next;
                elem_f->word->references--;
                delete elem_f;
                elem_f = elem_n;
            }

            first = nullptr;
            last = nullptr;
        }
    };

    /// special stack for word references that updates the referenced word's reference count
    template <size_t maxsize>
    class RefStack {
        Stack<param_t, maxsize> stack;
    public:
        void push (const param_t& ref) { all_words.get(ref).references++; stack.push(ref); }
        param_t& pop () { auto& ref = stack.pop(); all_words.get(ref).references--; return ref; }
        param_t& top () { return stack.top(); }
        size_t size() { return stack.size(); }
    };

    struct Bytecode {
        static const size_t bytecode_max_size = 1000;
        static const size_t literals_max_size = 1000;
        instr_t bytecode[bytecode_max_size];
        char literals[literals_max_size];
        param_t bytecode_last = 0;
        param_t literals_last = 0;

        // the state is the state of the bytecode's compilation
        struct state {
            enum {
                NONE,
                COMMENT,
                STRINGLITERAL,
                IFSTART,
                IFBLOCK,
                IFBLOCKEND,
                ELSESTART,
                ELSEBLOCK,
                BLOCKSTART,
                BLOCK,
                VECTOR,
                VECTORINT64,
                VECTORUINT64,
                VECTORFLOAT32,
                LAMBDA,
                LIST,
                PUSHINGWORD
            } state = NONE;
            param_t bytecode_offset = 0;
            param_t parameter = 0;
        };

        Stack<state, 50> stack;

        void append (const instr_t appendable) { bytecode[bytecode_last] = appendable; bytecode_last++; }
        void append (const param_t appendable) { *((param_t*)(&bytecode[bytecode_last])) = appendable; bytecode_last++; bytecode_last++;}
        void append (const void* const appendable, size_t length) { memcpy(&literals[literals_last], appendable, length); literals_last += length;}
        void insert (const param_t insertable, const param_t position) { *((param_t*)(&bytecode[position])) = insertable;}

        void compile (const char* code);
        void compile_token(const char* const token);
        void decompile ();

        Bytecode() { stack.push(state {state::NONE, 0}); }
    };

    struct InterpreterState {
        struct StackElement {
            Bytecode* bytecode;
            param_t bytecode_offset;
        };

        Stack<StackElement, 50> stack;  // subroutine stack
        RefStack<50> ref_stack;         // word stack

        void bytecode(Bytecode* bytecode) { stack.push(StackElement{bytecode, 0}); }
        void interpret();
    };

    struct ConsoleInterpreter {
        Bytecode b_code;
        InterpreterState i_state;
        bool continue_ = true;

        void interpret () {
            std::string inputs = " ";
            param_t b_code_offset = b_code.bytecode_last;

            while (inputs != "quit " && inputs != "exit " && continue_) {
                std::getline(std::cin, inputs);
                //std::cout << inputs << std::endl;
                if (inputs == "decompile") {
                    b_code.decompile();
                    continue;
                } else if (inputs == "collect") {
                    collect_garbage();
                    continue;
                }

                inputs += " "; // stupid hack

                b_code.compile(inputs.c_str());
                if (b_code.stack.top().state == Bytecode::state::NONE) {
                    i_state.stack.push(InterpreterState::StackElement { &b_code, b_code_offset});
                    i_state.interpret();

                    b_code_offset = b_code.bytecode_last;
                }
            }
        }
    };

    extern void (*native_calls[250])(InterpreterState&);


}

#endif //LIGMA_H