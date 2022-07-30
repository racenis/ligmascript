// intrpr_word_types.h
// No copyright intended.
//
// Typedefs for word types and some functions for working with them.

#ifndef WORD_TYPES_H
#define WORD_TYPES_H

// if changing the numbers, then make sure that the different bit lengths
// of a type are in order and one after another no gaps
#define WORD_VARIABLE 0
#define WORD_BYTECODE 1
#define WORD_LIST 2
#define WORD_STRING 3
#define WORD_INT8 4
#define WORD_INT16 5
#define WORD_INT32 6
#define WORD_INT64 7
#define WORD_UINT8 8
#define WORD_UINT16 9
#define WORD_UINT32 10
#define WORD_UINT64 11
// is threre even a float 8???
#define WORD_FLOAT8 12
#define WORD_FLOAT16 13
#define WORD_FLOAT32 14
#define WORD_FLOAT64 15

#define WORD_FLAG_ISBEGONIS 1
#define WORD_FLAG_READONLY 2
#define WORD_FLAG_NOCOLLECT 4
#define WORD_FLAG_NOFREE 8

static inline int word_is_int(short int wordtype){
    return
        wordtype == WORD_INT8 ||
        wordtype == WORD_INT16 ||
        wordtype == WORD_INT32 ||
        wordtype == WORD_INT64;
}

static inline int word_is_uint(short int wordtype){
    return
        wordtype == WORD_UINT8 ||
        wordtype == WORD_UINT16 ||
        wordtype == WORD_UINT32 ||
        wordtype == WORD_UINT64;
}

static inline int word_is_float(short int wordtype){
    return
        wordtype == WORD_FLOAT8 ||
        wordtype == WORD_FLOAT16 ||
        wordtype == WORD_FLOAT32 ||
        wordtype == WORD_FLOAT64;
}

static inline int word_length(short int wordtype){
    switch(wordtype){
    case WORD_INT8:
        return sizeof(signed char);
    case WORD_INT16:
        return sizeof(signed short);
    case WORD_INT32:
        return sizeof(signed int);
    case WORD_INT64:
        return sizeof(signed long);
    case WORD_UINT8:
        return sizeof(unsigned char);
    case WORD_UINT16:
        return sizeof(unsigned short);
    case WORD_UINT32:
        return sizeof(unsigned int);
    case WORD_UINT64:
        return sizeof(unsigned long);
    case WORD_FLOAT8:
        //printf("FLOAT8 not implemented.\n");
        return -1;
    case WORD_FLOAT16:
        //printf("FLOAT16 not implemented.\n");
        return -1;
    case WORD_FLOAT32:
        return sizeof(float);
    case WORD_FLOAT64:
        return sizeof(double);
    }
}

// checks if a word is a number (can be done arithmetic on)
static inline int word_is_number(short int wordtype){
	return word_is_int(wordtype) || word_is_uint(wordtype) || word_is_float(wordtype);
}

// compares two types and returns the betterest type
static inline int word_betterest(int word1type, int word2type){
    if((word_is_int(word1type) && word_is_int(word2type)) ||
       (word_is_uint(word1type) && word_is_uint(word2type)) ||
       (word_is_float(word1type) && word_is_float(word2type))){
            return word1type > word2type ? word1type : word2type;
        }

    if(word_is_float(word1type) || word_is_float(word2type)){
        return word_is_float(word1type) ? word1type : word2type;
    } else if (word_is_int(word1type) || word_is_int(word2type)){
        return word_is_int(word1type) ? word1type : word2type;
    } else if (word_is_uint(word1type) || word_is_uint(word2type)){
        return word_is_uint(word1type) ? word1type : word2type;
    } else {
        return -1;
    }
}

// there's probs a way betterer way to do this but whatevs
static inline void word_convert(short int typefrom, short int typeto, void* word, void* wordto){
    signed long w_s;
    unsigned long w_u;
    double w_d;

    if (word_is_int(typefrom)){
        switch(typefrom){
        case WORD_INT8:
            w_s = (signed long)(*(signed char*)word);
            break;
        case WORD_INT16:
            w_s = (signed long)(*(signed short*)word);
            break;
        case WORD_INT32:
            w_s = (signed long)(*(signed int*)word);
            break;
        case WORD_INT64:
            w_s = *((signed long*)word);
            break;
        }
        w_u = w_s;
        w_d = w_s;
    } else if (word_is_uint(typefrom)){
        switch(typefrom){
        case WORD_UINT8:
            w_u = (unsigned long)(*(unsigned char*)word);
            break;
        case WORD_UINT16:
            w_u = (unsigned long)(*(unsigned short*)word);
            break;
        case WORD_UINT32:
            w_u = (unsigned long)(*(unsigned int*)word);
            break;
        case WORD_UINT64:
            w_u = *((unsigned long*)word);
            break;
        }
        w_s = w_u;
        w_d = w_u;
    } if (word_is_float(typefrom)){
        switch(typefrom){
        case WORD_FLOAT8:
            //printf("FLOAT8 not implemented.\n");
            break;
        case WORD_FLOAT16:
            //printf("FLOAT16 not implemented.\n");
            break;
        case WORD_FLOAT32:
            w_d = *((float*)word);
            break;
        case WORD_FLOAT64:
            w_d = *((double*)word);
            break;
        }
        w_s = w_d;
        w_u = w_d;
    }



    switch(typeto){
    case WORD_INT8:
        *(signed char*)wordto = (signed char)w_s;
        break;
    case WORD_INT16:
        *(signed short*)wordto = (signed short)w_s;
        break;
    case WORD_INT32:
        *(signed int*)wordto = (signed int)w_s;
        break;
    case WORD_INT64:
        *(signed long*)wordto = w_s;
        break;
    case WORD_UINT8:
        *(unsigned char*)wordto = (unsigned char)w_s;
        break;
    case WORD_UINT16:
        *(unsigned short*)wordto = (unsigned short)w_s;
        break;
    case WORD_UINT32:
        *(unsigned int*)wordto = (unsigned int)w_s;
        break;
    case WORD_UINT64:
        *(unsigned long*)wordto = w_s;
        break;
    case WORD_FLOAT8:
        //printf("FLOAT8 not implemented.\n");
        break;
    case WORD_FLOAT16:
        //printf("FLOAT16 not implemented.\n");
        break;
    case WORD_FLOAT32:
        *(float*)wordto = (float)w_s;
        break;
    case WORD_FLOAT64:
        *(double*)wordto = (double)w_s;
        break;
    }

}

static inline const char* word_type_to_str(short int wordtype){
    switch(wordtype){
    case WORD_VARIABLE:
        return "VARIABLE";
        break;
    case WORD_BYTECODE:
        return "BYTECODE";
        break;
    case WORD_LIST:
        return "LIST";
        break;
    case WORD_STRING:
        return "STRING";
        break;
    case WORD_INT8:
        return "INT8";
        break;
    case WORD_INT16:
        return "INT16";
        break;
    case WORD_INT32:
        return "INT32";
        break;
    case WORD_INT64:
        return "INT64";
        break;
    case WORD_UINT8:
        return "UINT8";
        break;
    case WORD_UINT16:
        return "UINT16";
        break;
    case WORD_UINT32:
        return "UINT32";
        break;
    case WORD_UINT64:
        return "UINT64";
        break;
    case WORD_FLOAT8:
        return "FLOAT8";
        break;
    case WORD_FLOAT16:
        return "FLOAT16";
        break;
    case WORD_FLOAT32:
        return "FLOAT32";
        break;
    case WORD_FLOAT64:
        return "FLOAT64";
        break;
    default:
        return "UNKNOWN";
    }
}

#endif // WORD_TYPES_H
