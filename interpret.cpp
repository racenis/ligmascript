#include "ligma.h"

namespace ligma {
    void intp_comp_lessthan (InterpreterState& state);
    void intp_comp_greaterthan (InterpreterState& state);

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
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(0),
                1,
                0,
                false,
                false
        }));
        return;
        comp_true:
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(1),
                1,
                0,
                false,
                false
        }));
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
    for (int i = 0; i < 100; i++) {
        auto &b = stack.top();
        param_t &counter = b.bytecode_offset;
        instr_t &instr = *(b.bytecode->bytecode + b.bytecode_offset);
        param_t &param = *((param_t *) (b.bytecode->bytecode + b.bytecode_offset + 1));
        param_t &param2 = *((param_t *) (b.bytecode->bytecode + b.bytecode_offset + 3));

        switch (instr) {
            case EXECUTEWORD: {
                auto ref = all_words.get(param);

                if (ref.type == Word::BYTECODE) {
                    stack.push(StackElement{(Bytecode *) ref.value, (param_t) ref.size});
                } /*else if (ref.type != Word::UNDEFINED) {
                    // wtf why is this here
                    auto word = all_words.add(Word());
                    all_words.get(word).copy_from(ref);
                    ref_stack.push(word);
                }*/ else {
                    ref_stack.push(param);
                }

                counter += 3;
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

                if (condition.size != 1) throw Exception(Exception::INVALID_CONDITION);
                if (param == (param_t) -1) throw Exception(Exception::INVALID_JUMP_ADDRESS);
                if (!condition.value_as_uint(0)) {
                    counter = param;
                } else {
                    counter += 3;
                }
            }
                break;
            case ASSIGN: {
                auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());

                // TODO: add fast copy
                // if right has no refs after being popped, then you could just transfer
                // the data pointer to left and then yeet the right
                left.copy_from(right);

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
                        new List,
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

                if (left_w.type != Word::LIST) throw Exception (Exception::LIST_APPEND_NOT_LIST);

                ((List *) left_w.value)->append(right);

                ref_stack.push(left);

                counter++;
            }
                break;
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
            case EQUAL: {
                // TODO: move this into a seperate function
                stack_comparison<[](float a, float b){ return a == b; },
                        [](int64_t a, int64_t b){ return a == b; },
                        [](uint64_t a, uint64_t b){ return a == b; }>(*this);
                counter++;
                /*auto &right = all_words.get(ref_stack.pop());
                auto &left = all_words.get(ref_stack.pop());

                if (left.size != right.size) {
                    throw Exception(Exception::INCOMPATIBLE_COMPARISON_SIZES);
                }

                if (left.type_is_float() && left.type_is_float()) {
                    for (size_t i = 0; i < left.size; i++)
                        if (left.value_as_float(i) != right.value_as_float(i))
                            goto comp_false;
                    goto comp_true;
                } else if (left.type_is_int() && left.type_is_int()) {
                    throw Exception(Exception::NOT_IMPLEMENTED);
                } else if (left.type_is_uint() && left.type_is_uint()) {
                    throw Exception(Exception::NOT_IMPLEMENTED);
                } else {
                    throw Exception(Exception::INCOMPATIBLE_COMPARISON_TYPES);
                }

                comp_false:
                ref_stack.push(all_words.add(Word{
                        Word::UINT8,
                        new uint8_t(0),
                        1,
                        0,
                        false,
                        false
                }));
                goto comp_end;
                comp_true:
                ref_stack.push(all_words.add(Word{
                        Word::UINT8,
                        new uint8_t(1),
                        1,
                        0,
                        false,
                        false
                }));
                goto comp_end;
                comp_end:*/
                //counter++;
            }
                break;
            case LESSERTHAN:
                stack_comparison<[](float a, float b){ return a < b; },
                        [](int64_t a, int64_t b){ return a < b; },
                        [](uint64_t a, uint64_t b){ return a < b; }>(*this);
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
            case ADDITION:
                stack_operation_two<[](float a, float b){ return a + b; },
                        [](int64_t a, int64_t b){ return a + b; },
                        [](uint64_t a, uint64_t b){ return a + b; }>(*this);
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



    int64_t Word::value_as_int (size_t vec_index) {
        if (type_is_int()) {
            switch (type) {
                case INT64:
                    return *(((int64_t*)value) + vec_index);
                case INT32:
                    return *(((int32_t*)value) + vec_index);
                case INT16:
                    return *(((int16_t*)value) + vec_index);
                case INT8:
                    return *(((int8_t*)value) + vec_index);
                default:
                    return 0;
            }
        } else if (type_is_uint()) {
            return value_as_uint(vec_index);
        } else if (type_is_float()){
            return value_as_float(vec_index);
        } else {
            throw Exception(Exception::INVALID_TYPE_CONVERSION);
        }
    }

    uint64_t Word::value_as_uint (size_t vec_index) {
        if (type_is_int()) {
            return value_as_uint(vec_index);
        } else if (type_is_uint()) {
            switch (type) {
                case UINT64:
                    return *(((uint64_t*)value) + vec_index);
                    break;
                case UINT32:
                    return *(((uint32_t*)value) + vec_index);
                    break;
                case UINT16:
                    return *(((uint16_t*)value) + vec_index);
                    break;
                case UINT8:
                    return *(((uint8_t*)value) + vec_index);
                    break;
            }
        } else if (type_is_float()){
            return value_as_float(vec_index);
        } else {
            throw Exception(Exception::INVALID_TYPE_CONVERSION);
        }
    }

    float Word::value_as_float (size_t vec_index) {
        if (type_is_int()) {
            return value_as_int(vec_index);
        } else if (type_is_uint()) {
            return value_as_uint(vec_index);
        } else if (type_is_float()){
            switch (type) {
                case FLOAT64:
                    return *(((double*)value) + vec_index);
                    break;
                case FLOAT32:
                    return *(((float*)value) + vec_index);
                    break;
            }
        } else {
            throw Exception(Exception::INVALID_TYPE_CONVERSION);
        }
    }



    void intp_comp_lessthan (InterpreterState& state) {
        auto& right = all_words.get(state.ref_stack.pop());
        auto& left = all_words.get(state.ref_stack.pop());

        if (left.size != 1 || right.size != 1) {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_SIZES);
        }

        if (left.type_is_float() || left.type_is_float()) {
            if (left.value_as_float(0) < right.value_as_float(0)) goto comp_true; else goto comp_false;
        } else if (left.type_is_int() || left.type_is_int()) {
            if (left.value_as_int(0) < right.value_as_int(0)) goto comp_true; else goto comp_false;
        } else if (left.type_is_uint() || left.type_is_uint()) {
            if (left.value_as_uint(0) < right.value_as_uint(0)) goto comp_true; else goto comp_false;
        } else {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_TYPES);
        }

        comp_false:
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(0),
                1,
                0,
                false,
                false
        }));
        return;
        comp_true:
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(1),
                1,
                0,
                false,
                false
        }));
    }

    void intp_comp_greaterthan (InterpreterState& state) {
        auto& right = all_words.get(state.ref_stack.pop());
        auto& left = all_words.get(state.ref_stack.pop());

        if (left.size != 1 || right.size != 1) {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_SIZES);
        }

        if (left.type_is_float() || left.type_is_float()) {
            if (left.value_as_float(0) > right.value_as_float(0)) goto comp_true; else goto comp_false;
        } else if (left.type_is_int() || left.type_is_int()) {
            if (left.value_as_int(0) > right.value_as_int(0)) goto comp_true; else goto comp_false;
        } else if (left.type_is_uint() || left.type_is_uint()) {
            if (left.value_as_uint(0) > right.value_as_uint(0)) goto comp_true; else goto comp_false;
        } else {
            throw Exception(Exception::INCOMPATIBLE_COMPARISON_TYPES);
        }

        comp_false:
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(0),
                1,
                0,
                false,
                false
        }));
        return;
        comp_true:
        state.ref_stack.push(all_words.add(Word{
                Word::UINT8,
                new uint8_t(1),
                1,
                0,
                false,
                false
        }));
    }






    void collect_garbage () {
        for (param_t i = 0; i < all_words.size(); i++) {
            if (all_words.is_valid(i)) {
                all_words.get(i).print_info();
                std::cout << all_words.get(i).references << "refs, ";
                if(all_words.get(i).no_collect) std::cout << "nocollect" << std::endl << std::endl;
                    else std::cout << "collect" << std::endl << std::endl;

                if(!all_words.get(i).no_collect && all_words.get(i).references == 0) {
                    all_words.get(i).clean();
                    all_words.remove(i);
                }
            }
        }
    }

}