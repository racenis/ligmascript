#include "ligma.h"

namespace ligma {
    void Word::print_info() {
        switch (type) {
            case UNDEFINED:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case BYTECODE:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case LIST:
                if (value) {
                    std::cout << "[List START]" << std::endl;
                    auto element = ((List*) value)->first;
                    while (element) {
                        element->word->print_info();
                        element = element->next;
                    }
                    std::cout << "[List END]" << std::endl;
                } else {
                    std::cout << "Empty list" << std::endl;
                }
                break;
            case STRING:
                std::cout << "\"" << (char*)value << "\"" << std::endl;
                break;
            case INT64:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case INT32:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case INT16:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case INT8:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT64:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT32:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT16:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case UINT8:
                std::cout << type_as_str() << " : ";
                for (size_t i = 0; i < size; i++) std::cout << (uint16_t)*((uint8_t*)value + i) << " ";
                std::cout << std::endl;
                break;
            case FLOAT64:
                std::cout << "undefined conversion for type " << type_as_str() << std::endl;
                break;
            case FLOAT32:
                std::cout << type_as_str() << " : ";
                for (size_t i = 0; i < size; i++) std::cout << *((float*)value + i) << " ";
                std::cout << std::endl;
                break;
            default:
                break;
        }
    }

    const char* Word::type_as_str() const {
        switch (type) {
            case UNDEFINED:
                return "UNDEFINED";
            case BYTECODE:
                return "BYTECODE";
            case LIST:
                return "LIST";
            case STRING:
                return "STRING";
            case INT64:
                return "INT64";
            case INT32:
                return "INT32";
            case INT16:
                return "INT16";
            case INT8:
                return "INT8";
            case UINT64:
                return "UINT64";
            case UINT32:
                return "UINT32";
            case UINT16:
                return "UINT16";
            case UINT8:
                return "UINT8";
            case FLOAT64:
                return "FLOAT64";
            case FLOAT32:
                return "FLOAT32";
            default:
                return "UNKNOWN TYPE";
        }
        // TODO: add methods for type conversions
    }

    size_t Word::type_size() const {
        switch (type) {
            case INT64:
                return 8;
            case INT32:
                return 4;
            case INT16:
                return 2;
            case INT8:
                return 1;
            case UINT64:
                return 8;
            case UINT32:
                return 4;
            case UINT16:
                return 2;
            case UINT8:
                return 1;
            case FLOAT64:
                return 8;
            case FLOAT32:
                return 4;
            default:
                return 0;
        }
    }

    void Word::copy_from (Word& other) {
        clean();

        // TODO: add stuff from other types
        type = other.type;
        size = other.size;

        if (other.type_is_numeric() && other.value) {
            value = ::operator new(other.type_size() * other.size);
            mempcpy(value, other.value, other.type_size() * other.size);
            no_delete = false;
        } else if (other.type == Word::BYTECODE) {
            value = other.value;
            no_delete = true;
        } else {
            throw Exception("Assignment error.");
        }
    }

    void Word::clean () {
        if (!no_delete && value) {
            if (type == LIST) ((List*) value)->clean();
            ::operator delete (value);
        }
        value = nullptr;
    }
}