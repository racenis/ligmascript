
#include <unordered_map>
#include "ligma.h"

namespace ligma {
    std::unordered_map<std::string, param_t> word_names;
    Pool<Word, 500> all_words = {Word{}};
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
        try {
        bool list_appendable = false;

        if (stack.top().state == state::COMMENT) {
            if (strcmp(token, "\n") == 0) { stack.pop(); }
            return;
        }

        if (stack.top().state == state::BLOCKSTART) {
            if (strcmp(token, "vec") == 0) {
                stack.pop();
                stack.push(state{state::VECTOR});
                return;
            } else if (strcmp(token, "list") == 0) {
                stack.pop();
                stack.push(state{state::LIST});
                append(PUSH_LIST);
                return;
            } else if (strcmp(token, "lambda") == 0) {
                stack.pop();
                append(PUSH_LAMBDA);
                append((param_t) (bytecode_last + 5));
                append(JUMP);
                stack.push(state{state::LAMBDA, bytecode_last});
                append((param_t) -1);
                append(LINE);
                append(bytecode_line);
                return;
            } else {
                stack.pop();
                stack.push(state{state::BLOCK});
            }
        } else if (stack.top().state == state::VECTOR) {
            auto type = best_fit_numeric(token);

            if (type == Word::FLOAT32) {
                append(PUSHFLOAT32);
                append(literals_last);
                stack.pop();
                stack.push(state{state::VECTORFLOAT32});
            } else if (type == Word::INT64) {
                append(PUSHINT64);
                append(literals_last);
                stack.pop();
                stack.push(state{state::VECTORINT64});
            } else {
                append(PUSHUINT64);
                append(literals_last);
                stack.pop();
                stack.push(state{state::VECTORUINT64});
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
            if (strcmp(token, "'") == 0 && stack.top().bytecode_offset == 0 ||
                strcmp(token, "\"") == 0 && stack.top().bytecode_offset == 1) {
                append("\0", 1);
                append((param_t) (stack.pop().parameter + 1));
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
            if (strcmp(token, "(") != 0) {
                throw Exception(Exception::NOT_BRACKET_AFTER_IF);
            } else {
                stack.pop();
                append(JUMPIFNOT);
                stack.push(state{state::IFBLOCK, bytecode_last});
                append((param_t) -1);
                return;
            }
        }

        if (stack.top().state == state::ELSESTART) {
            if (strcmp(token, "(") != 0) {
                throw Exception(Exception::NOT_BRACKET_AFTER_ELSE);
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
                stack.push(state{state::ELSESTART, jump_param});
                return;
            } else {
                insert(bytecode_last, stack.pop().bytecode_offset);
                // don't return, continue processing token
            }
        }

        // this long chain of if/else's isn't very efficient, but it's simple and works
        if (false) {
        } else if (strcmp(token, ";") == 0) {
            stack.push(state{state::COMMENT});
        } else if (strcmp(token, "(") == 0) {
            stack.push(state{state::BLOCKSTART, bytecode_last});
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
        } else if (strcmp(token, "recurse") == 0) {
            append(JUMP);
            append((param_t)(stack.top().bytecode_offset + 2));
        } else if (strcmp(token, "[") == 0) {
            stack.push(state{state::PUSHINGWORD});
        } else if (strcmp(token, "]") == 0) {
            throw Exception(Exception::RANDOM_SQUARE_BRACKET);
        } else if (strcmp(token, "'") == 0 || strcmp(token, "\"") == 0) {
            if (strcmp(token, "'") == 0) {
                stack.push(state{state::STRINGLITERAL, 0});
            } else {
                stack.push(state{state::STRINGLITERAL, 1});
            }
            append(PUSH_STRING);
            append(literals_last);
            list_appendable = true;
        } else if (token[0] == '`') {
            append(PUSH_STRING);
            append(literals_last);
            param_t len = strlen(token);
            append(token + 1, len);
            append(len);
            list_appendable = true;
        } else if (strcmp(token, "car") == 0) {
            append(LISTDATA);
        } else if (strcmp(token, "cdr") == 0) {
            append(LISTNEXT);
        } else if (strcmp(token, "setcar") == 0) {
            append(LISTDATAASSIGN);
        } else if (strcmp(token, "setcdr") == 0) {
            append(LISTNEXTASSIGN);
        } else if (strcmp(token, ">>") == 0) {
            append(VECTOREXTRACT);
        } else if (strcmp(token, "<<") == 0) {
            append(VECTORINSERT);
        } else if (strcmp(token, "move") == 0) {
            append(MOVE);
        } else if (strcmp(token, "copy") == 0) {
            append(COPY);
        } else if (strcmp(token, ".s") == 0) {
            append(PRINTSTACK);
        } else if (strcmp(token, "if") == 0) {
            stack.push(state{state::IFSTART});
        } else if (strcmp(token, "print") == 0 || strcmp(token, ".") == 0) {
            append(PRINT);
        } else if (strcmp(token, "printinfo") == 0  || strcmp(token, "..") == 0) {
            append(PRINTINFO);
        } else if (strcmp(token, "+") == 0) {
            append(ADDITION);
        } else if (strcmp(token, "+") == 0) {
            append(ADDITION);
        } else if (strcmp(token, "-") == 0) {
            append(SUBTRACTION);
        } else if (strcmp(token, "*") == 0) {
            append(MULTIPLICATION);
        } else if (strcmp(token, "/") == 0) {
            append(DIVISION);
        } else if (strcmp(token, "**") == 0) {
            append(EXPONENT);
        } else if (strcmp(token, "/*") == 0) {
            append(MODULO);
        } else if (strcmp(token, "=") == 0) {
            append(EQUAL);
        } else if (strcmp(token, ">") == 0) {
            append(GREATERTHAN);
        } else if (strcmp(token, "<") == 0) {
            append(LESSERTHAN);
        } else if (strcmp(token, "=<") == 0) {
            append(LESSEROREQUAL);
        } else if (strcmp(token, ">=") == 0) {
            append(GREATEROREQUAL);
        } else if (strcmp(token, "and") == 0) {
            append(AND);
        } else if (strcmp(token, "or") == 0) {
            append(OR);
        } else if (strcmp(token, "set") == 0) {
            append(ASSIGN);
        } else if (strcmp(token, "call") == 0) {
            append(NATIVECALL);
        } else if (strcmp(token, "execute") == 0) {
            append(EXECUTEWORD);
        } else if (strcmp(token, "return") == 0) {
            append(RETURN);
        } else if (strcmp(token, "drop") == 0) {
            append(DROP);
        } else if (strcmp(token, "over") == 0) {
            append(OVER);
        } else if (strcmp(token, "swap") == 0) {
            append(SWAP);
        } else if (strcmp(token, "rot") == 0) {
            append(ROTATE);
        } else if (strcmp(token, "dup") == 0) {
            append(DUPLICATE);
        } else if (strcmp(token, ".s") == 0) {
            append(PRINTSTACK);
        } else if (strcmp(token, "nil") == 0) {
            append(PUSHWORD);
            append(UNINIT_REF);
            list_appendable = true;
        } else if (strcmp(token, "\n") == 0) {
            bytecode_line++;
            append(LINE);
            append(bytecode_line);
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
            append(PUSHWORD);
            append(find_word(token));
            append(EXECUTEWORD);
            list_appendable = true;
            //std::cout << "adding word";
        }

        if (stack.top().state == state::LIST) {
            if (!list_appendable) {
                throw Exception (Exception::LIST_UNAPPENDABLE);
            }
            append(LISTAPPEND);
        }
        } catch (Exception& e) {
            std::cout << "Compiler error: " << e.exceptstr() << " at line " << bytecode_line << " in " << name << std::endl;
        }
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

        if (stack.top().state == Bytecode::state::NONE) append(RETURN);
    }

}