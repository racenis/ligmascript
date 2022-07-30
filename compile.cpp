
#include <unordered_map>
#include "ligma.h"

namespace ligma {
    std::unordered_map<std::string, param_t> word_names;
    Pool<Word, 500> all_words;
    void (*native_calls[250])(InterpreterState&) = { nullptr };

    param_t find_word (const char* name) {
        auto it = word_names.find(name);

        if (it == word_names.end()) {
            auto id = all_words.add(Word {
                Word::UNDEFINED,
                nullptr,
                0,
                0,
                true,
                true
            });
            word_names[name] = id;
            return id;
        } else {
            return it->second;
        }
    }

    /// returns the best type for a numeric literal
    Word::Type best_fit_numeric (const char* token) {
        size_t token_len = strlen(token);
        if (token[token_len-1] == 'f') {
            return Word::FLOAT32;
        } else if (token[token_len-1] == 'i') {
            return Word::INT64;
        } else if (token[token_len-1] == 'u') {
            return Word::UINT64;
        } else {
            // TODO: add some more rules for determining best type
            return Word::FLOAT32;
        }
    }

    void Bytecode::compile_token(const char* const token) {
        bool list_appendable = false;

        if (stack.top().state == state::COMMENT) {
            if (strcmp(token, "\n") == 0) { stack.pop(); }
            return;
        }

        if (stack.top().state == state::BLOCKSTART) {
            if (strcmp(token, "vec") == 0) {
                stack.pop();
                stack.push(state { state::VECTOR });
                return;
            } else if (strcmp(token, "list") == 0) {
                stack.pop();
                stack.push(state { state::LIST });
                append(PUSH_LIST);
                return;
            } else if (strcmp(token, "lambda") == 0) {
                stack.pop();
                append(PUSH_LAMBDA);
                append((param_t)(bytecode_last + 5));
                append(JUMP);
                stack.push(state { state::LAMBDA, bytecode_last });
                append((param_t) -1);
                return;
            } else {
                stack.pop();
                stack.push(state { state::BLOCK });
            }
        } else if (stack.top().state == state::VECTOR) {
            auto type = best_fit_numeric(token);

            if (type == Word::FLOAT32) {
                append(PUSHFLOAT32);
                append(literals_last);
                stack.pop();
                stack.push(state {state::VECTORFLOAT32 });
            } else if (type == Word::INT64) {
                append(PUSHINT64);
                append(literals_last);
                stack.pop();
                stack.push(state {state::VECTORINT64 });
            } else {
                append(PUSHUINT64);
                append(literals_last);
                stack.pop();
                stack.push(state{state::VECTORUINT64 });
            }
            // note that we do not do a return here
        }

        if (stack.top().state == state::VECTORFLOAT32) {
            if (strcmp(token, ")") == 0) {
                append(stack.pop().parameter);
            } else {
                float amogus = std::atof(token);
                append(&amogus, sizeof(amogus));
                stack.top().parameter++;
            }
            return;
        } else if (stack.top().state == state::VECTORINT64) {
            if (strcmp(token, ")") == 0) {
                append(stack.pop().parameter);
            } else {
                int64_t amogus = std::atoll(token);
                append(&amogus, sizeof(amogus));
                stack.top().parameter++;
            }
            return;
        } else if (stack.top().state == state::VECTORUINT64) {
            if (strcmp(token, ")") == 0) {
                append(stack.pop().parameter);
            } else {
                uint64_t amogus = std::atoll(token);
                append(&amogus, sizeof(amogus));
                stack.top().parameter++;
            }
            return;
        }


        if (stack.top().state == state::STRINGLITERAL) {
            if (strcmp(token, "'") == 0 || strcmp(token, "\"") == 0) {
                append("\0", 1);
                append((param_t)(stack.pop().parameter + 1));
                return;
            } else {
                auto len = strlen(token);
                if (stack.top().parameter > 0) {
                    append(" ", 1);
                    stack.top().parameter += 1;
                }
                append(token, len);
                stack.top().parameter += len;
                return;
            }
        } else if (stack.top().state == state::PUSHINGWORD) {
            if (strcmp(token, "]") == 0) {
                stack.pop();
                return;
            } else {
                append(PUSHWORD);
                append(find_word(token));
                return;
            }
        }

        if (stack.top().state == state::IFSTART) {
            if(strcmp(token, "(") != 0) {
                throw Exception("Expected '(' after an 'if'.");
            } else {
                stack.pop();
                append(JUMPIFNOT);
                stack.push(state { state::IFBLOCK, bytecode_last});
                append((param_t) -1);
                return;
            }
        }

        if (stack.top().state == state::ELSESTART) {
            if(strcmp(token, "(") != 0) {
                throw Exception("Expected '(' after an 'else'.");
            } else {
                stack.top().state = state::ELSEBLOCK;
                return;
            }
        }

        if (stack.top().state == state::IFBLOCKEND) {
            if (strcmp(token, "else") == 0) {
                append(JUMP);
                auto jump_param = bytecode_last;
                append((param_t) -1);
                insert(bytecode_last, stack.pop().bytecode_offset);
                stack.push(state {state::ELSESTART, jump_param});
                return;
            } else {
                insert(bytecode_last, stack.pop().bytecode_offset);
                // don't return, continue processing token
            }
        }

        if (false) {
        } else if (strcmp(token, ";") == 0) {
            stack.push(state { state::COMMENT });
        } else if (strcmp(token, "(") == 0) {
            stack.push(state { state::BLOCKSTART, bytecode_last });
        } else if (strcmp(token, ")") == 0) {
            if (stack.top().state == state::LAMBDA) {
                append(RETURN);
                insert(bytecode_last, stack.pop().bytecode_offset);
            } else if (stack.top().state == state::IFBLOCK) {
                stack.top().state = state::IFBLOCKEND;
                //insert(bytecode_last, stack.pop().bytecode_offset);
            } else if (stack.top().state == state::ELSEBLOCK) {
                insert(bytecode_last, stack.pop().bytecode_offset);
            } else if (stack.top().state == state::LIST) {
                list_appendable = true;
                stack.pop();
            } else if (stack.top().state == state::BLOCK) {
                stack.pop();
            }
        } else if (strcmp(token, "[") == 0) {
            stack.push(state { state::PUSHINGWORD});
        } else if (strcmp(token, "]") == 0) {
            return;
        } else if (strcmp(token, "'") == 0 || strcmp(token, "\"") == 0) {
            stack.push(state { state::STRINGLITERAL });
            append(PUSH_STRING);
            append(literals_last);
            list_appendable = true;
        } else if (token[0] == '`') {
            append(PUSH_STRING);
            append(literals_last);
            param_t len = strlen(token);
            append(token + 1, len);
            append(len);
        } else if (strcmp(token, ".s") == 0) {
            append(PRINTSTACK);
        } else if (strcmp(token, "if") == 0) {
            stack.push(state { state::IFSTART });
        } else if (strcmp(token, "print") == 0) {
            append(PRINTINFO);
        } else if (strcmp(token, "+") == 0) {
            append(ADDITION);
        } else if (strcmp(token, "=") == 0) {
            append(EQUAL);
        } else if (strcmp(token, ">") == 0) {
            append(GREATERTHAN);
        } else if (strcmp(token, "<") == 0) {
            append(LESSERTHAN);
        } else if (strcmp(token, "set") == 0) {
            append(ASSIGN);
        } else if (strcmp(token, "call") == 0) {
            append(NATIVECALL);
        } else if (strcmp(token, ".s") == 0) {
            append(PRINTSTACK);
        } else if (strcmp(token, "\n") == 0) {
            //std::cout << "linebreak"; // TODO: add some kind of line break code here
        } else if ((token[0] >= '0' && token[0] <= '9') || token[0] == '-' || token[0] == '+') {
            auto type = best_fit_numeric(token);
            list_appendable = true;

            if (type == Word::FLOAT32) {
                float amogus = std::atof(token);
                append(PUSHFLOAT32);
                append(literals_last);
                append((param_t) 1);
                append(&amogus, sizeof(amogus));
            } else if (type == Word::INT64) {
                int64_t amogus = std::atoll(token);
                append(PUSHINT64);
                append(literals_last);
                append((param_t) 1);
                append(&amogus, sizeof(amogus));
            } else {
                uint64_t amogus = std::atoll(token);
                append(PUSHUINT64);
                append(literals_last);
                append((param_t) 1);
                append(&amogus, sizeof(amogus));
            }
        } else {
            // assuming that the token represents a word
            append(EXECUTEWORD);
            append(find_word(token));
            list_appendable = true;
            //std::cout << "adding word";
        }

        if (stack.top().state == state::LIST) {
            if (!list_appendable) {
                throw Exception ("List unappendable type");
            }
            append(LISTAPPEND);
        }

        //std::cout << std::endl;
    }

    void Bytecode::compile (const char* code) {
        // this method lexes the code and then forwards it to the compile function
        char buffer[500];
        char* buffer_last = buffer;

        for (; *code != '\0'; code++) {
            *buffer_last = *code;

            if (*code == '\n' || *code == ';' || *code == '(' || *code == ')' || *code == '[' || *code == ']' || *code == '\'' || *code == '"') {
                if (buffer_last != buffer) {
                    *buffer_last = '\0';
                    compile_token(buffer);
                }
                buffer[0] = *code;
                buffer[1] = '\0';
                compile_token(buffer);
                buffer_last = buffer;
                continue;
            } else if (*code == ' ' || *code == '\t') {
                if (buffer_last == buffer) continue; // ignore consecutive whitespaces
                *buffer_last = '\0';
                compile_token(buffer);
                buffer_last = buffer;
                continue;
            }

            buffer_last++;
        }

        append(RETURN);
    }



}