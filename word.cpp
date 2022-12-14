#include "ligma.h"

namespace ligma {
    void Word::print_info() {
        std::cout << "Type:\t" << type_as_str() << std::endl;
        std::cout << "Value:\t"; print(); std::cout << std::endl; // TODO: for strings don't format, just print directly
        std::cout << "Size:\t" << size << std::endl;
        std::cout << "Refs:\t" << references << std::endl;
        std::cout << "Flags:\t";
        if (!no_delete && !no_collect && !is_named) {
            std::cout << "none" << std::endl;
        } else {
            if (no_delete) std::cout << "no_delete ";
            if (no_collect) std::cout << "no_collect ";
            if (is_named) std::cout << "is_named ";
            std::cout << std::endl;
        }
    }
    void Word::print() {
        if (type_is_numeric() && size > 1) std::cout << "(";
        switch (type) {
            case UNDEFINED:
                std::cout << "nil";
                break;
            case BYTECODE:
                std::cout << "Bytecode " << ((Bytecode*)value)->name << " offset " << size << std::endl;
                break;
            case REFERENCE:
            case LIST:
                if (value) {
                    std::cout << "[";
                    if (car != UNINIT_REF) {
                        all_words.get(car).print();
                    }
                    auto element = cdr;
                    while (element != UNINIT_REF) {
                        std::cout << ", ";
                        all_words.get(all_words.get(element).car).print();
                        element = all_words.get(element).cdr;
                    }
                    std::cout << "]";
                } else {
                    std::cout << "Empty list";
                }
                break;
            case STRING:
            {
                char outp[500];
                char* s = (char*)value;
                char* d = outp;
                for (; *s != '\0'; s++, d++) {
                    if (*s == '%') {
                        s++;
                        if(*s == 't') {
                            *d = '\t';
                        } else if(*s == 'n') {
                            *d = '\n';
                        } else {
                            *d = *s;
                        }
                    } else {
                        *d = *s;
                    }
                }
                *d = '\0';
                std::cout << outp;
            }
                break;
            case INT64:
                for (size_t i = 0; i < size; i++) std::cout << *((int64_t*)value + i) << (i < size-1 ? ", " : "");
                break;
            case INT32:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case INT16:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case INT8:
                for (size_t i = 0; i < size; i++) std::cout << (int16_t)*((int8_t*)value + i) << (i < size-1 ? ", " : "");
                break;
            case UINT64:
                for (size_t i = 0; i < size; i++) std::cout << *((uint64_t*)value + i) << (i < size-1 ? ", " : "");
                break;
            case UINT32:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT16:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT8:
                for (size_t i = 0; i < size; i++) std::cout << (uint16_t)*((uint8_t*)value + i) << (i < size-1 ? ", " : "");
                break;
            case FLOAT64:
                for (size_t i = 0; i < size; i++) std::cout << *((double*)value + i) << (i < size-1 ? ", " : "");
                break;
            case FLOAT32:
                for (size_t i = 0; i < size; i++) std::cout << *((float*)value + i) << (i < size-1 ? ", " : "");
                break;
            default:
                break;
        }
        if (type_is_numeric() && size > 1) std::cout << ")";
    }

    void Word::copy_from (Word& other) {
        clean();

        type = other.type;
        size = other.size;

        if (other.type_is_numeric() && other.value) {
            // if the other things value is a no_delete, then
            // we don't have to memcpy, we can just use the
            // same pointer and it's going to be ok
            value = ::operator new(other.type_size() * other.size);
            mempcpy(value, other.value, other.type_size() * other.size);
            no_delete = false;
        } else if (other.type == Word::BYTECODE) {
            value = other.value;
            no_delete = true;
        } else if (other.type_is_reference()) {
            if(other.cdr != UNINIT_REF) all_words.get(other.cdr).references++;
            if(other.car != UNINIT_REF) all_words.get(other.car).references++;
            car = other.car;
            cdr = other.cdr;

            // TODO: add a string copy in here
        } else {
            throw Exception(Exception::INVALID_TYPE_ASSIGNMENT);
        }
    }

    void Word::cannibalize (Word& other) {
        clean();
        type = other.type;
        size = other.size;
        no_delete = other.no_delete;
        value = other.value;
        other.type = UNDEFINED;
        other.size = 0;
        other.value = nullptr;
    }

    void Word::make_reference_to (param_t word) {
        clean();
        type = REFERENCE;
        cdr = UNINIT_REF;
        car = word;
        all_words.get(word).references++;
    }

    void Word::clean () {
        if (type_is_reference()) {
            if (car != UNINIT_REF) all_words.get(car).references--;
            if (cdr != UNINIT_REF) all_words.get(cdr).references--;
        } else if (type != UNDEFINED && type != BYTECODE && !no_delete && value) {
            ::operator delete (value);
        }
        value = nullptr;
        no_delete = false;
        size = 0;
    }

    void Word::move_into (Word& other) {
        if (type_is_numeric() && size != other.size) {
            throw Exception(Exception::MOVE_VECTOR_DIFFERENT_SIZES);
        } else if (type == STRING && size < strlen((char*)other.value)+1) {
            throw Exception(Exception::MOVE_STRING_INSUFFICIENT_SIZE);
        }

        if (type_is_numeric()) {
            switch (type) {
                case FLOAT32:
                    for (size_t i = 0; i < size; i++) ((float*)value)[i] = other.value_as_float(i);
                    break;
                case FLOAT64:
                    for (size_t i = 0; i < size; i++) ((double*)value)[i] = other.value_as_float(i);
                    break;
                case INT64:
                    for (size_t i = 0; i < size; i++) ((int64_t*)value)[i] = other.value_as_int(i);
                    break;
                case INT32:
                    for (size_t i = 0; i < size; i++) ((int32_t*)value)[i] = other.value_as_int(i);
                    break;
                case INT16:
                    for (size_t i = 0; i < size; i++) ((int16_t*)value)[i] = other.value_as_int(i);
                    break;
                case INT8:
                    for (size_t i = 0; i < size; i++) ((int8_t*)value)[i] = other.value_as_int(i);
                    break;
                case UINT64:
                    for (size_t i = 0; i < size; i++) ((uint64_t*)value)[i] = other.value_as_uint(i);
                    break;
                case UINT32:
                    for (size_t i = 0; i < size; i++) ((uint32_t*)value)[i] = other.value_as_uint(i);
                    break;
                case UINT16:
                    for (size_t i = 0; i < size; i++) ((uint16_t*)value)[i] = other.value_as_uint(i);
                    break;
                case UINT8:
                    for (size_t i = 0; i < size; i++) ((uint8_t*)value)[i] = other.value_as_uint(i);
                    break;
            }
        } else if (type == STRING) {
            strcpy((char*)value, (char*)other.value);
        } else {
            throw Exception(Exception::MOVE_INVALID_TYPE);
        }
    }

    uint8_t Word::value_as_logic () {
        if (type_is_numeric()) {
            for (size_t i = 0; i < size; i++)
                if (value_as_uint(i) != 0) return 1;
            return 0;
        } else if (type_is_reference()) {
            return 1;
        } else if (type == STRING || type == BYTECODE) {
            return 1;
        } else {
            return 0;
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

}