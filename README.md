# KataScript
KataScript is a simple scripting language with familiar syntax, designed to be easily embedded in C++ applications. 

## Index
- [Tutorial](https://github.com/brwhale/KataScript/blob/main/README.md#tutorial)
  - [Variables](https://github.com/brwhale/KataScript/blob/main/README.md#variables)
  - [Functions](https://github.com/brwhale/KataScript/blob/main/README.md#functions)
  - [Lists](https://github.com/brwhale/KataScript/blob/main/README.md#lists)
  - [Loops](https://github.com/brwhale/KataScript/blob/main/README.md#loops)
  - [if/else](https://github.com/brwhale/KataScript/blob/main/README.md#ifelse)
  - [Errors](https://github.com/brwhale/KataScript/blob/main/README.md#errors)
- [Examples](https://github.com/brwhale/KataScript/blob/main/README.md#examples)
  - [Hello World](https://github.com/brwhale/KataScript/blob/main/README.md#hello-world)
  - [Fizzbuzz](https://github.com/brwhale/KataScript/blob/main/README.md#fizzbuzz)
  - [Fibonacci Series](https://github.com/brwhale/KataScript/blob/main/README.md#the-fibonacci-series)
- [C++ Integration](https://github.com/brwhale/KataScript/blob/main/README.md#c-integration)
  - [Invoke C++ From KataScript](https://github.com/brwhale/KataScript/blob/main/README.md#invoke-c-from-katascript)
  - [Invoke KataScript From C++](https://github.com/brwhale/KataScript/blob/main/README.md#invoke-katascript-from-c)

## Tutorial

### Comments
Comments are anything in a line after `//` and will be ignored.
>     // this is a comment
Comments need not start at the beginnning of a line.

### Variables
Simply attempt to use a variable and it will be created.

>     i = 5;

The variable `i` now stores the integer `5`.

If you want to store a float or a string in `i`, you can do that too.

>     i = 5.0; // i is a float
>     i = "string value"; // i is now a string

KataScript is weakly typed, so values will upconvert for operations when necessary and you can assign anything to anything.

### Functions
Functions are called with the syntax `name(arg(s)...)`. For example:

>     print(100);
>     print("hello ", "world");

Functions are created using the `func` keyword. Functions may return values, but it is not strictly required.

>     func add1(a) {
>       return a + 1;
>     }

### Lists
A list is a variable that contains a list of other variables and/or values
A list literal looks like `[value(s)...]` and a list access looks like `listname[index]` (note that lists are 0-indexed)

>     i = [1,2,3];
>     j = [4,5,6];
>     k = [7,8,9];
>     a = [i, j, k];
>     print(a);
>     // prints: 1, 2, 3, 4, 5, 6, 7, 8, 9
>     i = ["fish", "tacos"];
>     print(a);
>     // prints: fish, tacos, 4, 5, 6, 7, 8, 9
>     k = [7, "string", 9.0];
>     print(a);
>     // prints: fish, tacos, 4, 5, 6, 7, string, 9.000000

### Loops
`while()` and `for()` are synonyms that mean start a loop. Either way you can put 1-3 expressions seperated by semicolons inside the parens.
- 1 expression: behaves like a standard while loop
- 2 expressions: behaves like a for loop with no initialization statment
- 3 expressions: behaves like a standard for loop

Then just put the loop contents inside of curly brackets:

>     i = 20;
>     while (i > 0) {
>       print(--i);
>     }

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

### Errors
If an error is detected, the expression evaluation will be haled for the current line. Any subexpressions already evaluated are not undone. Error detection is currently basic and many errors will result in undefined behaviour instead.

>     j = 3;
>     print(j > 5);
>     // prints:  0
>     j = "7";
>     print(j > 5);
>     // prints: Error: Bad comparison comparing `STRING 7` to `INT 5`
>     j = 5;
>     print(++j > "5");
>     // prints: Error: Bad comparison comparing `INT 6` to `STRING 5`

>     print(j[0]);
>     // prints: 6
>     print(j[2]);
>     // prints: Error: Out of bounds list access index 2, list length 1

>     i = "hmmmm;
>     // prints: Error: Quote mismatch at "hmmmm;

These are currently the only 3 errors detected.

----

## Examples
### Hello World
>     print("hello world");

### Fizzbuzz
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

----

## C++ Integration
KataScript is a single header file. To include KataScript into your project simply put KataScript.hpp into your project, `#define KATASCRIPT_IMPL` in exactly `1` cpp file, and then #include it wherever you need.

>     #define KATASCRIPT_IMPL
>     #include "../../KataScript/src/Scripting/KataScript.hpp"

### Invoke C++ From KataScript
If we want to call C++ code from inside the script, we can register the code as a function any time after calling the KataScriptInterpreter constructor; (note that the function will be placed in the current scope)

Here's a function to wrap for the example: (in C++)
>     int integrationExample(int a) {
>       return a * a;
>     }

Then here's how we register it for use inside of KataScript: (note, this will overwrite any existing function with the same name, so you can use that to redirect the print function for example)
>     KataScript::KataScriptInterpreter interp;
>     interp.newFunction("integrationExample", [](KataScript::KSList args) {
>       // Dereference argument
>       auto val = *args[0];
>       // Coerce type
>       val.hardconvert(KataScript::KSType::INT);
>       // Call c++ code
>       auto result = integrationExample(val.getInt());
>       // Wrap and return
>       return std::make_shared<KataScript::KSValue>(result);
>     });

Now we can call that function from inside our scripts.

### Invoke KataScript From C++
We can also send commands from C++ into KataScript using the readLine function.
>     interp.readLine("i = integrationExample(4);");

Now `i` contains `16` and is an int.
If we want to pull that value out, we can do that too!

>     auto varRef = interp.resolveVariable("i");
>     
>     // visit style
>     std::visit([](auto&& arg) {std::cout << arg; }, varRef->value);
>     
>     // if the type is known
>     int i = varRef->getInt();
>     
>     // switch style
>     switch (varRef->type) {
>     case KataScript::KSType::INT:
>       std::cout << varRef->getInt();
>       break;
>     case KataScript::KSType::FLOAT:
>       std::cout << varRef->getFloat();
>       break;
>     case KataScript::KSType::STRING:
>       std::cout << varRef->getString();
>       break;
>     }

----
