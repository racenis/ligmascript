![LIGMAScript logo](/docs/ligma_logo_spacious.gif)

Very good programming language. The name stands for **LI**st **G**eneral **M**anipulation and **A**rithmetic **Script**. You can read more about it on the [github.io page](https://racenis.github.io/ligmascript/). 

### To-do list

- fix compiler making an error 
	- "list not list appendable" when a block (should be no error)
- add `==` operator
	- this operator would check if two references are the same, not their contents
	- also make `=` operator work on strings
- make values possible to be read-only
	- this would prevent accidentaly modifying literals with the `move` keyword
- use the same true/false value for all comparisons, instead of creating a new one every time
	- makes the intepreter go vroom vroom by yeeting unnecessary memory allocations

#### Stuff that will take some longer time
- namespaces
- a small standard library

## Example

#### Hello world

```
"hellow wworld uwu ;33" print 
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
	dup 0 = if (
		drop 0
	) else (
		dup 1 = if (
			drop 1
		) else (
			dup 1 - fibonacci
			over 2 - fibonacci
			+ swap drop
		)
	)
) set

"f(5):%t" . 5 fibonacci . `%n . ; will print 'f(5): 5'
"f(6):%t" . 6 fibonacci . `%n . ; will print 'f(6): 8'
"f(7):%t" . 7 fibonacci . `%n . ; will print 'f(7): 13'
```

## Compilation

Left as an exercise for the reader.

## Copyright

No copyright intended.