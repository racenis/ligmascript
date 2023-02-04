![LIGMAScript logo](/docs/ligma_logo_spacious.gif)

Very good programming language. The name stands for **LI**st **G**eneral **M**anipulation and **A**rithmetic **Script**. You can read more about it on the [github.io page](https://racenis.github.io/ligmascript/). 

### To-do list

- Rewrite tokenizer function
- Add labels
	- Add GOTO
- Think of some more stuff to add to the standard library
- Write some tests
- Add error checks to the compiler
	- There are no error checks
- Make compiled bytecode seriazibleble

- Create a proper runtime
	- Like node.js, but for LIGMAScript
	- Also write a text editor
	- And then a kernel, just in case

#### Stuff that will take some longer time
- namespaces
- a small standard library

## Example

#### Hello world

```
"hellow wworld" . cr  
```

#### Reverse linked list

```
reverse-list (lambda 
	dup cdr dup
	if (
		over nil setcdr
		(lambda 
			dup cdr rot rot
			swap over swap setcdr
			swap dup if () else (drop return)
			recurse
		) execute 
	) else (return)
) set

(list 1 2 3 4 5 6 7 8 9 10) dup
"linked list:%t" . . ; will print 'linked list: [1, 2, 3 ...]'
reverse-list
"%nin reverse:%t" . . ; will print 'in reverse: [10, 9, 8 ...]'
```

### Fibonacci sequence

```
fibonacci (lambda 
	dup 0 == if (
		drop 0
	) else (
		dup 1 == if (
			drop 1
		) else (
			dup 1 - fibonacci
			over 2 - fibonacci
			+ swap drop
		)
	)
) set

"f(5): " . 5 fibonacci . cr ; will print 'f(5): 5'
"f(6): " . 6 fibonacci . cr ; will print 'f(6): 8'
"f(7): " . 7 fibonacci . cr ; will print 'f(7): 13'
```

## Compilation

I have no makefiles. Dump all of the .cpp files from the /src/ directory into 
your favorite IDE or makefile generator. 

If you want to embed the interpreter/compiler into your own program, then 
exclude the main.cpp from the static library. 

If you want to run LIGMAScript in a REPL loop, then do include the main.cpp and 
compile everything as an executable. You might want to edit the main.cpp to make 
the runtime load in all of the LIGMAScript standard library.

The only dependencies are the C++ standard library. I have only tested it on the 
MinGW/gcc compiler, but it should work with any compiler that supports C++98.

## Copyright

No copyright intended.