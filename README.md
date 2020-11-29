# KataScript
A simple scripting language with familiar syntax.

## Tutorial:

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
>     add("hello ", "world");

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
If we want to call C++ code from inside the script, we can register the code as a function any time after calling the KataScriptInterpereter constructor; (note that the function will be placed in the current scope)

Here's a function to wrap for the example: (in C++)
>     int integrationExample(int a) {
>       return a * a;
>     }

Then here's how we register it for use inside of KataScript:
>     interp.newFunction("integrationExample", [](KataScript::KSFunctionArgs args) {
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
