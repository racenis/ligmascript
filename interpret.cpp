#include <cmath>
#include "ligma.h"

namespace ligma {
    void logic_value (InterpreterState& state, const uint8_t& value) {
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(value),
                1,
                0,
                false,
                false
        }));
    }

    template <auto float_comp, auto int_comp, auto uint_comp>
    void stack_comparison (InterpreterState& state) {
        auto& right = all_words.get(state.ref_stack.pop());
        auto& left = all_words.get(state.ref_stack.pop());

        if (left.size != 1 || right.size != 1) {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_SIZES);
        }

        if (left.type_is_float() || left.type_is_float()) {
            if (float_comp(left.value_as_float(0), right.value_as_float(0))) goto comp_true; else goto comp_false;
        } else if (left.type_is_int() || left.type_is_int()) {
            if (int_comp(left.value_as_int(0), right.value_as_int(0))) goto comp_true; else goto comp_false;
        } else if (left.type_is_uint() || left.type_is_uint()) {
            if (uint_comp(left.value_as_uint(0), right.value_as_uint(0))) goto comp_true; else goto comp_false;
        } else {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_TYPES);
        }

        comp_false:
        logic_value(state, false);
        return;
        comp_true:
        logic_value(state, true);
    }

    template <auto float_op, auto int_op, auto uint_op>
    void stack_operation_two (InterpreterState& state) {
        auto &right = all_words.get(state.ref_stack.pop());
        auto &left = all_words.get(state.ref_stack.pop());

        if (left.size != right.size && right.size != 1) {
            throw Exception(Exception::INCOMPATIBLE_ARITHMETIC_SIZES);
        }

        if (left.type_is_float() || right.type_is_float()) {
            //float* value = new float[left.size];
            float *value = (float *) ::operator new(left.size * sizeof(float));
            size_t r_add = right.size == 1 ? 0 : 1;
            for (size_t i = 0, k = 0; i < left.size; i++, k += r_add)
                value[i] = float_op(left.value_as_float(i), right.value_as_float(k));
            state.ref_stack.push(all_words.add(Word{
                    Word::FLOAT32,
                    value,
                    left.size,
                    0,
                    false,
                    false
            }));
        } else if (left.type_is_int() || right.type_is_int()) {
            //int64_t* value = new int64_t[left.size];
            int64_t *value = (int64_t *) ::operator new(left.size * sizeof(int64_t));
            size_t r_add = right.size == 1 ? 0 : 1;
            for (size_t i = 0, k = 0; i < left.size; i++, k += r_add)
                value[i] = int_op(left.value_as_int(i), right.value_as_int(k));
            state.ref_stack.push(all_words.add(Word{
                    Word::INT64,
                    value,
                    left.size,
                    0,
                    false,
                    false
            }));
        } else if (left.type_is_uint() || right.type_is_uint()) {
            //uint64_t* value = new uint64_t[left.size];
            int64_t *value = (int64_t *) ::operator new(left.size * sizeof(int64_t));
            size_t r_add = right.size == 1 ? 0 : 1;
            for (size_t i = 0, k = 0; i < left.size; i++, k += r_add)
                value[i] = uint_op(left.value_as_uint(i), right.value_as_uint(k));
            state.ref_stack.push(all_words.add(Word{
                    Word::UINT64,
                    value,
                    left.size,
                    0,
                    false,
                    false
            }));
        } else {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_TYPES);
        }
    }

    void InterpreterState::interpret() {
try {
    for (int i = 0; i < 1000; i++) {
        auto &b = stack.top();
        param_t &counter = b.bytecode_offset;
        instr_t &instr = *(b.bytecode->bytecode + b.bytecode_offset);
        param_t &param = *((param_t *) (b.bytecode->bytecode + b.bytecode_offset + 1));
        param_t &param2 = *((param_t *) (b.bytecode->bytecode + b.bytecode_offset + 3));

        switch (instr) {
            case EXECUTEWORD: {
                auto ref_n = ref_stack.pop();
                auto &ref = all_words.get(ref_n);

                if (ref.type == Word::BYTECODE) {
                    stack.push(StackElement{(Bytecode *) ref.value, (param_t) ref.size});
                } else if (ref.type == Word::REFERENCE) {
                    if (ref.car != UNINIT_REF && all_words.get(ref.car).type == Word::BYTECODE) {
                        auto& bcode = all_words.get(ref.car);
                        stack.push(StackElement{(Bytecode *) bcode.value, (param_t) bcode.size});
                    } else {
                        ref_stack.push(ref.car);
                    }
                } else {
                    ref_stack.push(ref_n);
                }

                counter++;
            }
                break;
            case PUSHWORD: {
                ref_stack.push(param);
                counter += 3;
            }
                break;
            case NATIVECALL: {
                auto &index = all_words.get(ref_stack.pop());
                if (index.value_as_uint(0) >= sizeof(native_calls)
                    || native_calls[index.value_as_uint(0)] == nullptr) {
                    throw Exception (Exception::UNDEFINED_NATIVE_CALL);
                } else {
                    native_calls[index.value_as_uint(0)](*this);
                }
                counter++;
            }
                break;
            case JUMP: {
                if (param == (param_t) -1) throw Exception(Exception::INVALID_JUMP_ADDRESS);
                counter = param;
            }
                break;
            case LINE: {
                stack.top().bytecode_line = param;
                counter += 3;
            }
                break;
            case JUMPIFNOT: {
                auto &condition = all_words.get(ref_stack.pop());

                if (param == (param_t) -1) throw Exception(Exception::INVALID_JUMP_ADDRESS);
                if (!condition.value_as_logic()) {
                    counter = param;
                } else {
                    counter += 3;
                }
            }
                break;
            case ASSIGN: {
                auto right_n = ref_stack.pop();
                auto &right = all_words.get(right_n);
                auto &left = all_words.get(ref_stack.pop());

                // TODO: when read_only stuff is added, then you should always use copy_from for them
                if (right.references == 0 && !right.is_named) {
                    left.cannibalize(right);
                } else if (right.type == Word::BYTECODE) {
                    left.copy_from(right);
                } else {
                    left.make_reference_to(right_n);
                }

                counter++;
            }
                break;
            case PUSH_LAMBDA: {
                ref_stack.push(all_words.add(Word{
                        Word::BYTECODE,
                        stack.top().bytecode,
                        param,
                        0,
                        true,
                        false
                }));
                counter += 3;
            }
                break;
            case PUSH_LIST: {
                ref_stack.push(all_words.add(Word{
                        Word::LIST,
                        nullptr,
                        0,
                        0,
                        false,
                        false
                }));
                counter++;
            }
                break;
            case LISTAPPEND: {
                auto right = ref_stack.pop();
                auto left = ref_stack.pop();
                auto &left_w = all_words.get(left);

                if (!left_w.type_is_reference()) throw Exception (Exception::LIST_APPEND_NOT_LIST);
                if (left_w.car == UNINIT_REF) left_w.car = right;
                else {
                    auto n_list = all_words.add(Word{
                            Word::LIST,
                            nullptr,
                            0,
                            1,
                            false,
                            false
                    });
                    all_words.get(n_list).car = right;
                    all_words.get(n_list).cdr = UNINIT_REF;
                    auto appndbl = left;
                    while (all_words.get(appndbl).cdr != UNINIT_REF) appndbl = all_words.get(appndbl).cdr;
                    all_words.get(appndbl).cdr = n_list;
                }

                all_words.get(right).references++;
                ref_stack.push(left);

                counter++;
            }
                break;
            case LISTDATA: {
                auto &word = all_words.get(ref_stack.pop());

                if (word.type_is_reference()) {
                    ref_stack.push(word.car);
                } else {
                    ref_stack.push(UNINIT_REF);
                }

                counter++;
            } break;
            case LISTNEXT: {
                auto &word = all_words.get(ref_stack.pop());

                if (word.type_is_reference()) {
                    ref_stack.push(word.cdr);
                } else {
                    ref_stack.push(UNINIT_REF);
                }

                counter++;
            } break;
            case LISTDATAASSIGN: {
                auto right = ref_stack.pop();
                auto left = ref_stack.pop();
                auto &right_w = all_words.get(right);
                auto &left_w = all_words.get(left);

                if (!left_w.type_is_reference()) throw Exception(Exception::LIST_DATA_ASSIGN_NOT_LIST);
                if (left_w.car != UNINIT_REF) all_words.get(left_w.car).references--;
                left_w.car = right;
                right_w.references++;
                counter++;
            } break;
            case LISTNEXTASSIGN: {
                auto right = ref_stack.pop();
                auto left = ref_stack.pop();
                auto &right_w = all_words.get(right);
                auto &left_w = all_words.get(left);

                if (!left_w.type_is_reference()) throw Exception(Exception::LIST_NEXT_ASSIGN_NOT_LIST);
                if (left_w.cdr != UNINIT_REF) all_words.get(left_w.cdr).references--;
                left_w.cdr = right;
                right_w.references++;
                counter++;
            } break;
            case VECTOREXTRACT: {
                auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());

                if (left.type != Word::FLOAT32 && left.type != Word::INT64 && left.type != Word::UINT64) {
                    throw Exception(Exception::VECTOR_EXTRACT_UNSUPPORTED_TYPE);
                }

                auto extract_index = right.value_as_int(0);
                if (extract_index < 0 || extract_index >= left.size) {
                    throw Exception(Exception::VECTOR_EXTRACT_INVALID_INDEX);
                }

                void* new_value;
                Word::Type new_word_type;

                if (left.type_is_float()) {
                    new_word_type = Word::FLOAT32;
                    new_value = new float(left.value_as_float(extract_index));
                } else if (left.type_is_int()) {
                    new_word_type = Word::INT64;
                    new_value = new int64_t(left.value_as_int(extract_index));
                } else if (left.type_is_uint()) {
                    new_word_type = Word::UINT64;
                    new_value = new uint64_t(left.value_as_uint(extract_index));
                }

                ref_stack.push(all_words.add(Word{
                        new_word_type,
                        new_value,
                        1,
                        0,
                        false,
                        false
                }));

                counter++;
            } break;
            case VECTORINSERT: {
                auto &right = all_words.get(ref_stack.pop());
                auto &middle = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());

                if (right.size != 1){
                    throw Exception(Exception::VECTOR_INSERT_VECTOR);
                }

                if (left.type != Word::FLOAT32 && left.type != Word::INT64 && left.type != Word::UINT64) {
                    throw Exception(Exception::VECTOR_INSERT_UNSUPPORTED_TYPE);
                }

                auto insert_index = right.value_as_int(0);
                if (insert_index < 0 || insert_index >= left.size) {
                    throw Exception(Exception::VECTOR_INSERT_INVALID_INDEX);
                }

                auto new_word_ref = all_words.add(Word{});
                auto& new_word = all_words.get(new_word_ref);
                new_word.copy_from(left);
                switch(left.type){
                    // maybe factor this out as a 'Word' class method?
                    case Word::FLOAT32:
                        ((float*)new_word.value)[insert_index] = middle.value_as_float(0);
                    break;
                    case Word::INT64:
                        ((int64_t*)new_word.value)[insert_index] = middle.value_as_int(0);
                        break;
                    case Word::UINT64:
                        ((uint64_t*)new_word.value)[insert_index] = middle.value_as_uint(0);
                        break;
                }

                ref_stack.push(new_word_ref);
                counter++;
            } break;
            case MOVE: {
                auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());

                left.move_into(right);
                counter++;
            } break;
            case COPY: {
                auto &word = all_words.get(ref_stack.pop());
                auto new_word_ref = all_words.add(Word{});
                auto &new_word = all_words.get(new_word_ref);

                new_word.copy_from(word);

                ref_stack.push(new_word_ref);
                counter++;
            } break;
            case PUSHFLOAT32: {
                ref_stack.push(all_words.add(Word{
                        Word::FLOAT32,
                        stack.top().bytecode->literals + param,
                        param2,
                        0,
                        true,
                        false
                }));
                counter += 5;
            }
                break;
            case PUSHINT64: {
                ref_stack.push(all_words.add(Word{
                        Word::INT64,
                        stack.top().bytecode->literals + param,
                        param2,
                        0,
                        true,
                        false
                }));
                counter += 5;
            }
                break;
            case PUSHUINT64: {
                ref_stack.push(all_words.add(Word{
                        Word::UINT64,
                        stack.top().bytecode->literals + param,
                        param2,
                        0,
                        true,
                        false
                }));
                counter += 5;
            }
                break;
            case PUSH_STRING: {
                ref_stack.push(all_words.add(Word{
                        Word::STRING,
                        stack.top().bytecode->literals + param,
                        param2,
                        0,
                        true,
                        false
                }));
                counter += 5;
            }
                break;
            case DROP: {
                ref_stack.pop();
                counter++;
            } break;
            case SWAP: {
                auto right = ref_stack.pop();
                auto left = ref_stack.pop();
                ref_stack.push(right);
                ref_stack.push(left);
                counter++;
            } break;
            case OVER: {
                auto right = ref_stack.pop();
                auto left = ref_stack.pop();
                ref_stack.push(left);
                ref_stack.push(right);
                ref_stack.push(left);
                counter++;
            } break;
            case ROTATE: {
                auto right = ref_stack.pop();
                auto middle = ref_stack.pop();
                auto left = ref_stack.pop();
                ref_stack.push(middle);
                ref_stack.push(right);
                ref_stack.push(left);
                counter++;
            } break;
            case DUPLICATE: {
                auto right = ref_stack.pop();
                ref_stack.push(right);
                ref_stack.push(right);
                counter++;
            } break;
            case EQUAL: {
                stack_comparison<[](float a, float b){ return a == b; },
                        [](int64_t a, int64_t b){ return a == b; },
                        [](uint64_t a, uint64_t b){ return a == b; }>(*this);
                counter++;
            }
                break;
            case LESSERTHAN:
                stack_comparison<[](float a, float b){ return a < b; },
                        [](int64_t a, int64_t b){ return a < b; },
                        [](uint64_t a, uint64_t b){ return a < b; }>(*this);
                counter++;
                break;
            case LESSEROREQUAL:
                stack_comparison<[](float a, float b){ return a <= b; },
                        [](int64_t a, int64_t b){ return a <= b; },
                        [](uint64_t a, uint64_t b){ return a <= b; }>(*this);
                counter++;
                break;
            case GREATERTHAN:
                // if you don't do it likes this, then it will not compile
                static const auto comp_gt_float = [](float a, float b){ return a > b; };
                static const auto comp_gt_int = [](int64_t a, int64_t b){ return a > b; };
                static const auto comp_gt_uint = [](uint64_t a, uint64_t b){ return a > b; };
                stack_comparison<comp_gt_float, comp_gt_int, comp_gt_uint>(*this);
                counter++;
                break;
            case GREATEROREQUAL:
                stack_comparison<[](float a, float b){ return a >= b; },
                        [](int64_t a, int64_t b){ return a >= b; },
                        [](uint64_t a, uint64_t b){ return a >= b; }>(*this);
                counter++;
                break;
            case AND: {
                auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());
                logic_value(*this, right.value_as_logic() && left.value_as_logic());
                counter++;
            } break;
            case OR: {
                auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());
                logic_value(*this, right.value_as_logic() || left.value_as_logic());
                counter++;
            } break;
            case ADDITION:
                stack_operation_two<[](float a, float b){ return a + b; },
                        [](int64_t a, int64_t b){ return a + b; },
                        [](uint64_t a, uint64_t b){ return a + b; }>(*this);
                counter++;
                break;
            case SUBTRACTION:
                stack_operation_two<[](float a, float b){ return a - b; },
                        [](int64_t a, int64_t b){ return a - b; },
                        [](uint64_t a, uint64_t b){ return a - b; }>(*this);
                counter++;
                break;
            case MULTIPLICATION:
                stack_operation_two<[](float a, float b){ return a * b; },
                        [](int64_t a, int64_t b){ return a * b; },
                        [](uint64_t a, uint64_t b){ return a * b; }>(*this);
                counter++;
                break;
            case DIVISION:
                stack_operation_two<[](float a, float b){ return a / b; },
                        [](int64_t a, int64_t b){ return a / b; },
                        [](uint64_t a, uint64_t b){ return a / b; }>(*this);
                counter++;
                break;
            case MODULO:
                stack_operation_two<[](float a, float b){ return std::fmod(a, b); },
                        [](int64_t a, int64_t b){ return a % b; },
                        [](uint64_t a, uint64_t b){ return a % b; }>(*this);
                counter++;
                break;
            case EXPONENT:
                stack_operation_two<[](float a, float b){ return std::pow(a, b); },
                        [](int64_t a, int64_t b){ return std::pow(a, b); },
                        [](uint64_t a, uint64_t b){ return std::pow(a, b); }>(*this);
                counter++;
                break;
            case PRINTSTACK: {
                Stack<param_t, 50> ref_restack;
                while (ref_stack.size() != 0) { ref_restack.push(ref_stack.pop()); }
                while (ref_restack.size() != 0) {
                    std::cout << "[" << ref_restack.top() << " : " <<
                              all_words.get(ref_restack.top()).type_as_str() << "] ";
                    ref_stack.push(ref_restack.pop());
                }
                if (ref_stack.size() == 0) std::cout << "empty";
                std::cout << std::endl;

                counter++;
            }
                break;
            case PRINTINFO: {
                auto var = ref_stack.pop();
                all_words.get(var).print_info();
                counter++;
            }
                break;
            case PRINT: {
                auto var = ref_stack.pop();
                all_words.get(var).print();
                counter++;
            }
                break;
            case RETURN:
                stack.pop();
                if (stack.size() == 0) return;
                break;
            default:
                std::cout << "Unrecognized instruction " << bytecode_instruction_str[instr] << " ["
                          << (uint16_t) instr << "]" << std::endl;
                counter++;
        }
    }
    std::cout << "END!" << std::endl;
    } catch (Exception& e) {
            std::cout << "Interpreter error: " << e.exceptstr() << "." << std::endl;
            while (stack.size()) {
                auto& s = stack.top();
                std::cout << "At line " << s.bytecode_line << " in " << s.bytecode->name << std::endl;
                stack.pop();
            }
        }
    }

    void collect_garbage () {
        for (param_t i = 0; i < all_words.size(); i++) {
            if (all_words.is_valid(i)) {
                auto& word = all_words.get(i);
                bool collect = word.references == 0 && !word.no_collect && !word.is_named;

                std::cout << (collect ? "[COLLECT] " : "[  NOT  ] ") << "Ref num:\t" << i << " Value: \t";
                word.print(); std::cout << std::endl;

                if (collect) {
                    all_words.get(i).clean();
                    all_words.remove(i);
                }
            }
        }
    }

}