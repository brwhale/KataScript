# KataScript
A simple scripting language with familiar syntax.

## Tutorial:

### Variables
Simply attempt to use a variable and it will be created.

>     i = 5;

The variable `i` now stores the integer `5`.

If you want to store a float or a string in `i`, you can do that too.

>     i = 5.0;
>     i = "string value";

KataScript is weakly typed, so values will upconvert for operations when necessary and you can assign anything to anything.

### Functions
Functions are called with the syntax `name(arg(s)...)`. For example:

>     print(100);

Functions are created using the `func` keyword. Functions may return values, but it is not strictly required.

>     func add1(a) {
>       return a + 1;
>     }

### Loops
`while()` and `for()` are synonyms that mean start a loop. Either way you can put 1-3 expressions seperated by semicolons inside the parens.
- 1 expression: behaves like a standard while loop
- 2 expressions: behaves like a for loop with no initialization statment
- 3 expressions: behaves like a standard for loop

Then just put the loop contents inside of curly brackets:

>     for (i = 0; i < 100; i++) {
>       print(i);
>     }

### if/else
`if`/`else if`/`else` all work how you'd expect.

>     if (5 == 5) { 
>       print("pie"); 
>     } else if (5 == 6) { 
>       print("cake"); 
>     } else { 
>       print("coffee"); 
>     }

----

## Examples
### Hello World
>     print("hello world");

### fizzbuzz
>     func fizzbuzz(n) {
>       for(i=1;i<=n;i++) { 
>         if (i % 15 == 0) { 
>           print("fizzbuzz"); 
>         } else if (i % 3 == 0) { 
>           print("fizz"); 
>         } else if (i % 5 == 0) { 
>           print("buzz"); 
>         } else { 
>           print(i);
>         } 
>       }
>     }

### The Fibonacci Series
>     func fib(c) {
>       i = 0;
>       j = 1;
>       while(i<c) { 
>         print(i); 
>         i += j; 
>         a = i;
>         i = j;
>         j = a;
>       }
>     }
