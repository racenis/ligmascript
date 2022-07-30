#include <intrpr_interpreter.h>
#include <intrpr_word_types.h>

void interpreter_garbage_collect(){
    for (int i = 0; i < interpreter_last_word; i++){
        struct interpreter_word* w = &interpreter_words[i];
        printf("Wordtype: %s Refs: %d", word_type_to_str(w->word_type), w->refs);
        if(w->flags & WORD_FLAG_NOCOLLECT)printf(" NOCOLLECT");
        if(w->flags & WORD_FLAG_NOFREE)printf(" NOFREE");
        printf("\n");
    }

}
