#include <interpreter/word.h>
#include <interpreter/interpreter.h>
#include <interpreter/error.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

namespace ligma {
    bool Word::is_external() {
        return flags & FLAG_EXT;
    }
    
    bool Word::is_int() {
        return type == WORD_INT
            || type == WORD_EXT_INT8
            || type == WORD_EXT_INT16
            || type == WORD_EXT_INT32
            || type == WORD_EXT_INT64;
    }
    
    bool Word::is_float() {
        return type == WORD_FLOAT
            || type == WORD_EXT_FLOAT32
            || type == WORD_EXT_FLOAT64;
    }
    
    bool Word::is_numeric() {
        return is_int() || is_float();
    }
    
    bool Word::is_string() {
        return type == WORD_STRING;
    }
    
    /// Returns the value as an int.
    /// @note   If the word's type isn't WORD_INT, you need to free() the value
    ///         after using it.
    /// @return Pointer to an int_t value.
    int32_t* Word::value_as_int() {
        if (type == WORD_INT) {
            return int_value;
        }
        
        if (type == WORD_FLOAT) {
            int32_t* value = (int32_t*) malloc (length * sizeof(int32_t));
            for (int i = 0; i < length; i++) value[i] = float_value[i];
            return value;
        }
        
        if (is_numeric()) {
            return extract_int();
        } else {
            throw InterpreterError (INTERPRETER_ERROR_UNCONVERTABLE_TO_INT);
        }
        
        return NULL;
    }
    
    /// Extracts an external numeric as an int.
    int32_t* Word::extract_int () {
        assert(is_external() && is_numeric());
        
        int32_t* value = (int32_t*) malloc (length * sizeof(int32_t));
        
        switch (type) {
            case WORD_EXT_INT8:     for (int i = 0; i < length; i++) value[i] = ((int8_t*)external_value)[i];   break;
            case WORD_EXT_INT16:    for (int i = 0; i < length; i++) value[i] = ((int16_t*)external_value)[i];  break;
            case WORD_EXT_INT32:    for (int i = 0; i < length; i++) value[i] = ((int32_t*)external_value)[i];  break;
            case WORD_EXT_INT64:    for (int i = 0; i < length; i++) value[i] = ((int64_t*)external_value)[i];  break;
            case WORD_EXT_FLOAT32:  for (int i = 0; i < length; i++) value[i] = ((float*)external_value)[i];    break;
            case WORD_EXT_FLOAT64:  for (int i = 0; i < length; i++) value[i] = ((double*)external_value)[i];   break;
            default:                assert(false);
        }
        
        return value;
    }
    
    /// Extracts an external numeric as a float.
    float* Word::extract_float () {
        assert(is_external() && is_numeric());
        
        float* value = (float*) malloc (length * sizeof(float));
        
        switch (type) {
            case WORD_EXT_INT8:     for (int i = 0; i < length; i++) value[i] = ((int8_t*)external_value)[i];   break;
            case WORD_EXT_INT16:    for (int i = 0; i < length; i++) value[i] = ((int16_t*)external_value)[i];  break;
            case WORD_EXT_INT32:    for (int i = 0; i < length; i++) value[i] = ((int32_t*)external_value)[i];  break;
            case WORD_EXT_INT64:    for (int i = 0; i < length; i++) value[i] = ((int64_t*)external_value)[i];  break;
            case WORD_EXT_FLOAT32:  for (int i = 0; i < length; i++) value[i] = ((float*)external_value)[i];    break;
            case WORD_EXT_FLOAT64:  for (int i = 0; i < length; i++) value[i] = ((double*)external_value)[i];   break;             
            default:                assert(false);
        }
        
        return value;
    }
    
    void Word::deposit_int(int32_t* value) {
        assert(is_external() && is_numeric());
        
        switch (type) {
            case WORD_EXT_INT8:     for (int i = 0; i < length; i++) ((int8_t*)external_value)[i] = value[i];   break;
            case WORD_EXT_INT16:    for (int i = 0; i < length; i++) ((int16_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_INT32:    for (int i = 0; i < length; i++) ((int32_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_INT64:    for (int i = 0; i < length; i++) ((int64_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_FLOAT32:  for (int i = 0; i < length; i++) ((float*)external_value)[i] = value[i];    break;
            case WORD_EXT_FLOAT64:  for (int i = 0; i < length; i++) ((double*)external_value)[i] = value[i];   break;               
            default:                assert(false);
        }
    }
    
    void Word::deposit_float(float* value) {
        assert(is_external() && is_numeric());
        
        switch (type) {
            case WORD_EXT_INT8:     for (int i = 0; i < length; i++) ((int8_t*)external_value)[i] = value[i];   break;
            case WORD_EXT_INT16:    for (int i = 0; i < length; i++) ((int16_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_INT32:    for (int i = 0; i < length; i++) ((int32_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_INT64:    for (int i = 0; i < length; i++) ((int64_t*)external_value)[i] = value[i];  break;
            case WORD_EXT_FLOAT32:  for (int i = 0; i < length; i++) ((float*)external_value)[i] = value[i];    break;
            case WORD_EXT_FLOAT64:  for (int i = 0; i < length; i++) ((double*)external_value)[i] = value[i];   break;               
            default:                assert(false);
        }
    }
    
    void Word::deposit_string(char* value) {
        strcpy((char*)external_value, value);
    }
    
    /// Returns the value as a float.
    /// @note   If the word's type isn't WORD_FLOAT, you need to free() the value
    ///         after using it.
    /// @return Pointer to a float_t value.
    float* Word::value_as_float() {
        if (type == WORD_FLOAT) {
            return float_value;
        }
        
        if (type == WORD_INT) {
            float* value = (float*) malloc (length * sizeof(float));
            for (int i = 0; i < length; i++) value[i] = int_value[i];
            return value;
        }
        
        if (is_numeric()) {
            return extract_float();
        } else {
            throw InterpreterError (INTERPRETER_ERROR_UNCONVERTABLE_TO_FLOAT);
        }
        
        return NULL;
    }
    
    /// Returns the value as a string.
    /// @note   If the word's type isn't WORD_STRING, you need to free() the value
    ///         after using it.
    /// @return Pointer to a str_t.
    char* Word::value_as_string() {
        if (type == WORD_STRING) {
            return string_value;
        }
        
        char* value = (char*) malloc(20 * (length + 1));    // idk
        char* it = value;
        char* ptr = string_value;                           // or whatever, it's all union'ed anyway
        
        if (type == WORD_ATOM) {
            strcpy(value, LookupAtom(atom_value)); return value;
        }
        
        if (length == 0) {
            strcpy (value, "NOT STRINGABLE"); return value;
        }
        
        if (type == WORD_LIST) {
            sprintf(it, "nxt:%d val:%d", list_value.next, list_value.data); return value;
        }
        
        if (length > 1) *it++ = '(';
        
        for (int i = 0; i < length; i++) {
            if (i > 0) {
                strcat(it, ", "); it += 2;
            }
            
            switch (type) {
                case WORD_INT:          it += sprintf(it, "%d", *((int_t*) ptr));      ptr += sizeof(int_t);   break;
                case WORD_FLOAT:        it += sprintf(it, "%.1f", *((float*) ptr));     ptr += sizeof(float);   break;
                case WORD_EXT_INT8:     it += sprintf(it, "%d", *((int8_t*) ptr));      ptr += sizeof(int8_t);  break;
                case WORD_EXT_INT16:    it += sprintf(it, "%d", *((int16_t*) ptr));     ptr += sizeof(int16_t); break;
                case WORD_EXT_INT32:    it += sprintf(it, "%d", *((int32_t*) ptr));     ptr += sizeof(int32_t); break;
                case WORD_EXT_INT64:    it += sprintf(it, "%d", *((int32_t*) ptr));     ptr += sizeof(int64_t); break;
                case WORD_EXT_FLOAT32:  it += sprintf(it, "%.1f", *((float*) ptr));     ptr += sizeof(float);   break;
                case WORD_EXT_FLOAT64:  it += sprintf(it, "%.1f", *((double*) ptr));    ptr += sizeof(double);  break;
                default:                strcpy (value, "NOT STRINGABLE");               return value;
            }
        }
    
        if (length > 1) *it++ = ')';
        
        *it = '\0';
        
        return value;
    }
    
    /// Inserts into a vector.
    void Word::insert_int (int_t v, offset_t i) {
        assert(type == WORD_INT);
        assert(i < length);
        this->int_value[i] = v;
    }
    
    /// Inserts into a vector.
    void Word::insert_float (float_t v, offset_t i) {
        assert(type == WORD_FLOAT);
        assert(i < length);
        this->float_value[i] = v;
    }
    
    /// Extracts from a vector.
    int_t Word::uninsert_int (offset_t i) {
        assert(type == WORD_INT);
        assert(i < length);
        return this->int_value[i];
    }
    
    /// Extracts from a vector.
    float_t Word::uninsert_float (offset_t i) {
        assert(type == WORD_FLOAT);
        assert(i < length);
        return this->float_value[i];
    }
    
    
    /// Makes Word a copy of an another Word.
    void Word::make_copy (Word* p) {
        if (p->is_numeric()) {
            if (p->is_int()) {
                int_t* val = p->value_as_int();
                size_t val_size = p->length * sizeof(int_t);
                this->int_value = (int_t*) malloc (val_size);
                memcpy(this->int_value, val, val_size);
                if (p->type != WORD_INT) free (val);
                this->type = WORD_INT;
                this->length = p->length;
            } else {
                float_t* val = p->value_as_float();
                size_t val_size = p->length * sizeof(float_t);
                this->float_value = (float_t*) malloc (val_size);
                memcpy(this->float_value, val, val_size);
                if (p->type != WORD_FLOAT) free (val);
                this->type = WORD_FLOAT;
                this->length = p->length;
            }
        } else {
            switch (p->type) {
            case WORD_ATOM:
                this->atom_value = p->atom_value;
                break;
            case WORD_STRING:
                this->string_value = (char*) malloc (strlen(p->string_value) + 1);
                strcpy(this->string_value, p->string_value);
                break;
            case WORD_BYTECODE:
                this->bytecode_value = p->bytecode_value;
                break;
            case WORD_LIST:
                this->list_value = p->list_value;
                if (this->list_value.data) LookupWord(this->list_value.data)->refs++;
                if (this->list_value.next) LookupWord(this->list_value.next)->refs++;
                break;
            default:
                assert(false); // throw error about copying
            }
            
            this->type = p->type;
            this->length = p->length;
        }
    }
    
    /// Makes a word a result of an addition.
    /// Adds up the numeric value of p1 and p2, stores it into the word.
    void Word::make_result_addition (Word* p1, Word* p2) {
        if (!p1->is_numeric() || !p2->is_numeric()) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_NON_NUMERIC);
        }
        
        if (p1->length != p2->length && p2->length != 1) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_INCOMPATIBLE_LENGTHS);
        }
        
        if (p1->is_float() || p2->is_float()) {
            float_t* v1 = p1->value_as_float();
            float_t* v2 = p2->value_as_float();
            float_t* res = (float_t*) malloc (p1->length * sizeof(float_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] + *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] + v2[i];
            }
            
            if (p1->type != WORD_FLOAT) free (v1);
            if (p2->type != WORD_FLOAT) free (v2);
            
            type = WORD_FLOAT;
            length = p1->length;
            float_value = res;
        } else {
            int_t* v1 = p1->value_as_int();
            int_t* v2 = p2->value_as_int();
            int_t* res = (int_t*) malloc (p1->length * sizeof(int_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] + *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] + v2[i];
            }
            
            type = WORD_INT;
            length = p1->length;
            int_value = res;
        }
    }
    
    /// Makes a word a result of an subtraction.
    /// Subtracts the numeric value of p2 from p1, stores it into the word.
    void Word::make_result_subtraction (Word* p1, Word* p2) {
        if (!p1->is_numeric() || !p2->is_numeric()) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_NON_NUMERIC);
        }
        
        if (p1->length != p2->length && p2->length != 1) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_INCOMPATIBLE_LENGTHS);
        }
        
        if (p1->is_float() || p2->is_float()) {
            float_t* v1 = p1->value_as_float();
            float_t* v2 = p2->value_as_float();
            float_t* res = (float_t*) malloc (p1->length * sizeof(float_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] - *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] - v2[i];
            }
            
            if (p1->type != WORD_FLOAT) free (v1);
            if (p2->type != WORD_FLOAT) free (v2);
            
            type = WORD_FLOAT;
            length = p1->length;
            float_value = res;
        } else {
            int_t* v1 = p1->value_as_int();
            int_t* v2 = p2->value_as_int();
            int_t* res = (int_t*) malloc (p1->length * sizeof(int_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] - *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] - v2[i];
            }
            
            type = WORD_INT;
            length = p1->length;
            int_value = res;
        }
    }
    
    /// Makes a word a result of a multiplication.
    /// Multiplies the numeric value of p1 and p2, stores it into the word.
    void Word::make_result_multiplication (Word* p1, Word* p2) {
        if (!p1->is_numeric() || !p2->is_numeric()) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_NON_NUMERIC);
        }
        
        if (p1->length != p2->length && p2->length != 1) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_INCOMPATIBLE_LENGTHS);
        }
        
        if (p1->is_float() || p2->is_float()) {
            float_t* v1 = p1->value_as_float();
            float_t* v2 = p2->value_as_float();
            float_t* res = (float_t*) malloc (p1->length * sizeof(float_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] * *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] * v2[i];
            }
            
            if (p1->type != WORD_FLOAT) free (v1);
            if (p2->type != WORD_FLOAT) free (v2);
            
            type = WORD_FLOAT;
            length = p1->length;
            float_value = res;
        } else {
            int_t* v1 = p1->value_as_int();
            int_t* v2 = p2->value_as_int();
            int_t* res = (int_t*) malloc (p1->length * sizeof(int_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] * *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] * v2[i];
            }
            
            type = WORD_INT;
            length = p1->length;
            int_value = res;
        }
    }
    
    /// Makes a word a result of a division.
    /// Divides the numeric value of p1 with p2, stores it into the word.
    void Word::make_result_division (Word* p1, Word* p2) {
        if (!p1->is_numeric() || !p2->is_numeric()) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_NON_NUMERIC);
        }
        
        if (p1->length != p2->length && p2->length != 1) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_INCOMPATIBLE_LENGTHS);
        }
        
        if (p1->is_float() || p2->is_float()) {
            float_t* v1 = p1->value_as_float();
            float_t* v2 = p2->value_as_float();
            float_t* res = (float_t*) malloc (p1->length * sizeof(float_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] / *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] / v2[i];
            }
            
            if (p1->type != WORD_FLOAT) free (v1);
            if (p2->type != WORD_FLOAT) free (v2);
            
            type = WORD_FLOAT;
            length = p1->length;
            float_value = res;
        } else {
            int_t* v1 = p1->value_as_int();
            int_t* v2 = p2->value_as_int();
            int_t* res = (int_t*) malloc (p1->length * sizeof(int_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] / *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] / v2[i];
            }
            
            type = WORD_INT;
            length = p1->length;
            int_value = res;
        }
    }
    
    /// Makes a word a result of a modulo.
    /// Divides numeric value of p1 with p2, stores the remainded into the word.
    void Word::make_result_modulo (Word* p1, Word* p2) {
        if (!p1->is_numeric() || !p2->is_numeric()) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_NON_NUMERIC);
        }
        
        if (p1->length != p2->length && p2->length != 1) {
            throw InterpreterError (INTERPRETER_ERROR_ADDITION_INCOMPATIBLE_LENGTHS);
        }
        
        if (p1->is_float() || p2->is_float()) {
            float_t* v1 = p1->value_as_float();
            float_t* v2 = p2->value_as_float();
            float_t* res = (float_t*) malloc (p1->length * sizeof(float_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = fmod(v1[i], *v2);
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = fmod(v1[i], v2[i]);
            }
            
            if (p1->type != WORD_FLOAT) free (v1);
            if (p2->type != WORD_FLOAT) free (v2);
            
            type = WORD_FLOAT;
            length = p1->length;
            float_value = res;
        } else {
            int_t* v1 = p1->value_as_int();
            int_t* v2 = p2->value_as_int();
            int_t* res = (int_t*) malloc (p1->length * sizeof(int_t));
            
            if (p2->length == 1) {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] % *v2;
            } else {
                for (int i = 0; i < p1->length; i++) res[i] = v1[i] % v2[i];
            }
            
            type = WORD_INT;
            length = p1->length;
            int_value = res;
        }
    }
    
    /// Clears the word's value.
    void Word::clear() {
        if (flags & FLAG_READONLY) return;
        
        if (!(flags & FLAG_LITERAL || flags & FLAG_EXT)) {
            switch (type) {
                case WORD_ATOM:                             break;
                case WORD_STRING:   free (string_value);    break;
                case WORD_INT:      free (int_value);       break;
                case WORD_FLOAT:    free (float_value);     break;
                case WORD_LIST:
                    if (list_value.data) LookupWord(list_value.data)->refs--;
                    if (list_value.next) LookupWord(list_value.next)->refs--;
                default:                                    break;
            }
        }
        
        type = WORD_ATOM;
        length = 0;
        atom_value = 0;
    }
    
    const char* Word::type_as_string() {
        switch (type) {
            case WORD_ATOM:             return "ATOM";
            case WORD_STRING:           return "STRING";
            case WORD_INT:              return "INT";
            case WORD_FLOAT:            return "FLOAT";
            case WORD_BYTECODE:         return "LAMBDA";
            case WORD_LIST:             return "LIST";
            case WORD_EXT_INT8:         return "INT8";
            case WORD_EXT_INT16:        return "INT16";
            case WORD_EXT_INT32:        return "INT32";
            case WORD_EXT_INT64:        return "INT64";
            case WORD_EXT_FLOAT32:      return "FLOAT32";
            case WORD_EXT_FLOAT64:      return "FLOAT64";
            case WORD_EXT_STRING:       return "CSTRING";
            case WORD_EXT_FUNCTION:     return "FUNCTION";
        }
        
        return NULL;
    }
    
    bool Word::is_equal_to (Word* other) {
        if (this->length != other->length) {
            return false;
        }
        
        if (this->type == WORD_ATOM && other->type == WORD_ATOM) {
            return this->atom_value == other->atom_value;
        }

        if (this->is_int() && other->is_int()) {
            int_t* val1 = this->value_as_int();
            int_t* val2 = other->value_as_int();
            bool is_equal = true;

            for (int i = 0; i < this->length; i++) {
                if (val1[i] != val2[i]) {
                    is_equal = false;
                    break;
                }
            }

            if (this->type != WORD_INT) free (val1);
            if (other->type != WORD_INT) free (val2);
            
            return is_equal;
        } else if (this->is_numeric() && other->is_numeric()) {
            float_t* val1 = this->value_as_float();
            float_t* val2 = other->value_as_float();
            bool is_equal = true;

            for (int i = 0; i < this->length; i++) {
                if (val1[i] != val2[i]) {
                    is_equal = false;
                    break;
                }
            }

            if (this->type != WORD_FLOAT) free (val1);
            if (other->type != WORD_FLOAT) free (val2);
            
            return is_equal;
        } else if (this->is_string() && other->is_string()) {
            return strcmp(this->string_value, other->string_value) == 0;
        }
        
        return false;
    }
    
    bool Word::is_not_equal_to (Word* other) {
        return !is_equal_to(other);
    }
    
    bool Word::is_greater_than (Word* other) {
        if (this->length != other->length) {
            return false;
        }

        if (this->is_int() && other->is_int()) {
            int_t* val1 = this->value_as_int();
            int_t* val2 = other->value_as_int();
            bool is_equal = true;

            for (int i = 0; i < this->length; i++) {
                if (!(val1[i] > val2[i])) {
                    is_equal = false;
                    break;
                }
            }

            if (this->type != WORD_INT) free (val1);
            if (other->type != WORD_INT) free (val2);
            
            return is_equal;
        } else if (this->is_numeric() && other->is_numeric()) {
            float_t* val1 = this->value_as_float();
            float_t* val2 = other->value_as_float();
            bool is_equal = true;

            for (int i = 0; i < this->length; i++) {
                if (!(val1[i] > val2[i])) {
                    is_equal = false;
                    break;
                }
            }

            if (this->type != WORD_FLOAT) free (val1);
            if (other->type != WORD_FLOAT) free (val2);
            
            return is_equal;
        }
        
        return false; // TODO: make an error
    }
    
    bool Word::is_lesser_than (Word* other) {
        return !is_greater_than(other) && !is_equal_to(other);
    }
    
    bool Word::is_greater_or_equal_to (Word* other) {
        return is_greater_than(other) || is_equal_to(other);
    }
    
    bool Word::is_lesser_or_equal_to (Word* other) {
        return !is_greater_than(other);
    }
    

}