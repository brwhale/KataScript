# KataScript
KataScript is a simple scripting language with familiar syntax, designed to be easily embedded in C++ applications.

[In-Browser Demo](https://brwhale.github.io/KataScript/)

## Index
- [Overview](https://github.com/brwhale/KataScript/blob/main/README.md#overview)
  - [Purpose](https://github.com/brwhale/KataScript/blob/main/README.md#purpose)
  - [Values and Types](https://github.com/brwhale/KataScript/blob/main/README.md#values-and-types)
  - [Type Coercion](https://github.com/brwhale/KataScript/blob/main/README.md#typecoercion)
  - [Variables](https://github.com/brwhale/KataScript/blob/main/README.md#variables)
  - [Memory](https://github.com/brwhale/KataScript/blob/main/README.md#memory)
  - [Special Characters](https://github.com/brwhale/KataScript/blob/main/README.md#special-characters)
  - [Keywords](https://github.com/brwhale/KataScript/blob/main/README.md#keywords)
  - [Comments](https://github.com/brwhale/KataScript/blob/main/README.md#comments)
- [Collections](https://github.com/brwhale/KataScript/blob/main/README.md#collections)
  - [Arrays](https://github.com/brwhale/KataScript/blob/main/README.md#arrays)
  - [Lists](https://github.com/brwhale/KataScript/blob/main/README.md#lists)
  - [Vec3](https://github.com/brwhale/KataScript/blob/main/README.md#vec3)
- [Control Flow](https://github.com/brwhale/KataScript/blob/main/README.md#control-flow)
  - [Functions](https://github.com/brwhale/KataScript/blob/main/README.md#functions)
  - [Loops](https://github.com/brwhale/KataScript/blob/main/README.md#loops)
  - [if/else](https://github.com/brwhale/KataScript/blob/main/README.md#ifelse)
- [Errors](https://github.com/brwhale/KataScript/blob/main/README.md#errors)
- [Built-in Functions](https://github.com/brwhale/KataScript/blob/main/README.md#built-in-functions)
  - [Math Operators](https://github.com/brwhale/KataScript/blob/main/README.md#math-operators)
  - [Comparison Operators](https://github.com/brwhale/KataScript/blob/main/README.md#comparison-operators)
  - [Alias Functions](https://github.com/brwhale/KataScript/blob/main/README.md#alias-functions)
  - [Other Functions](https://github.com/brwhale/KataScript/blob/main/README.md#other-functions)
  - [Precedence](https://github.com/brwhale/KataScript/blob/main/README.md#precedence)
- [Examples](https://github.com/brwhale/KataScript/blob/main/README.md#examples)
  - [Hello World](https://github.com/brwhale/KataScript/blob/main/README.md#hello-world)
  - [Fizzbuzz](https://github.com/brwhale/KataScript/blob/main/README.md#fizzbuzz)
  - [Fibonacci Series](https://github.com/brwhale/KataScript/blob/main/README.md#the-fibonacci-series)
  - [Functional Programming](https://github.com/brwhale/KataScript/blob/main/README.md#functional-programming)
- [C++ Integration](https://github.com/brwhale/KataScript/blob/main/README.md#c-integration)
  - [Invoke C++ From KataScript](https://github.com/brwhale/KataScript/blob/main/README.md#invoke-c-from-katascript)
  - [Invoke KataScript From C++](https://github.com/brwhale/KataScript/blob/main/README.md#invoke-katascript-from-c)
  - [C++ Types and Methods](https://github.com/brwhale/KataScript/blob/main/README.md#c-types-and-methods)
  - [C++ Usage Pattern](https://github.com/brwhale/KataScript/blob/main/README.md#c-usage-pattern)

## Overview

### Purpose
KataScript is designed to be lightweight, secure, sane, and easy to use. I made KataScript because I wanted scripting for my game engine but I did not enjoy the quirks of Lua or the security issues of including a Python or JavaScript environment. Therefore the goal of KataScript is to provide the sanest possible embedded scripting with the least amount of hurdles.

### Values and Types
Like most scripting languages, KataScript is dynamically typed. 

Values can currently have 8 different types: `none`, `int`, `float`, `vec3`, `function`, `string`, `array`, and `list`. 

`none`: the default type of any values. Converts to 0

`int`: 32 bit signed integers supplied by underlying C++ int type. Boolean logic uses the int type like C, 0 = False, anything else = True.

`float`: 32 bit signed floating point IEEE-754 supplied by underlying C++ float type.

`vec3`: 3 floats wraped into a struct. Used for 3d vectors, math, etc. Directly casts to glm::vec3

`function`: A function reference, can be assigned, called, and passed around but not compared or converted.

`string`: A mutable UTF8 character string supplied by underlying C++ std::string type. (some unicode doesn't work in the demo though because of the JavaScript interface)

`array`: A collection of other values. An array must be homogeneous (all values of the same type) and cannot contain other collections, supplied by underlying C++ std::vector containing values contiguously and represented in the unboxed base type. Array data is acessed by integer index starting from 0.

`list`: A collection of other values. A list can be heterogeneous and contain other lists, supplied by underlying C++ std::vector containing references to values. List data is acessed by integer index starting from 0. Slower than an `array` but more flexible.

### Type Coercion
There is minimal type coercion in KataScipt. Int will be promoted to Float for any operation between the two types, and String will be promoted to a List of characters when operating with a List. Attempting to compare a number type to a string or a list (unless it's only 1 element long and that element is a number) will throw an error. If you want to purposely convert values, there are built in casting/parsing functions `bool()`, `int()`, `float()`, `string()`, `array()`, `list()`.

### Variables
Simply attempt to use a variable and it will be created in the current scope.

>     i = 5;

The variable `i` now stores the integer `5`.

If you want to store a float or a string in `i`, you can do that too.

>     i = 5.0; // i is a float
>     i = "string value"; // i is now a string

If you run into scoping issues, you can also use the `var` keyword to ensure a new variable is created. Variables created inside functions should use the var keyword to avoid issues when using higher order functions.

>     var i = 3;

### Memory
KataScript is not technically garbage collected but does offer automatic memory management. It uses reference counting and scoping to free memory immediately after a value isn't being used anymore.

### Special Characters
`"`, `,`, `.`, `=`, `!`, `&`, `|`, `+`, `-`, `%`, `*`, `/`, `\`, `<`, `>`, `(`, `)`, `[`, `]`, `{`, `}`, and `;` are characters with special meaning to the parser and thus cannot be used in identifiers.

Whitespace outside of strings is stripped before parsing. 
Quotation marks inside a string must be escaped like `\"`

### Keywords
`if`, `else`, `func`, `var`, `return`, `foreach`, `for`, and `while` are reserved for control flow constructs and cannot be used as an identifier.

`true`, `false`, and `null` are reserved for constants and cannot be used as an identifier.

### Comments
Comments are anything in a line after `//` that isn't in a string literal and will be ignored.
>     // this is a comment
Comments need not start at the beginnning of a line.

## Collections
A collection literal looks like `[value(s)...]` and a collection access looks like `name[index]` (note that collections are 0-indexed). A collection literal will resolve to an `array` if all the types are the same and a `list` otherwise.
The `length()` function can tell you the size of a collection.

### Arrays
An array is a collection of unboxed values. All values in an array must be the same type and that type cannot be a collection.

>     i = [1, 2, 3]; // array from literal
>     i = array(1, "fish", 2, 3); // array function
>     // array type is the type of the first element added.
>     print(i[1]);
>     // prints: 2, note how "fish" was not added to the array
>     print(length(i));
>     // prints: 3, again only the integer values were accepted

### Lists
A list is a collection of boxed value references. A list can store values of any type and can store other collections.

>     i = [1.0, 2, 3, "squid"]; // list from literal
>     i = list(1, 2, 3); // list function
>     print(i[1]);
>     // prints: 2
>     print(length(i));
>     // prints: 3
>     j = list(4, 5, 6);
>     k = list(7, 8, 9);
>     a = list(i, j, k);
>     print(a);
>     // prints: 1, 2, 3, 4, 5, 6, 7, 8, 9
>     i = list("fish", "tacos");
>     print(a);
>     // prints: fish, tacos, 4, 5, 6, 7, 8, 9
>     k = list(7, "string", 9.0);
>     print(a);
>     // prints: fish, tacos, 4, 5, 6, 7, string, 9.000000

Note how list elements follow reference semantics, if you wish to actually copy the data into a list, you can use casting operators to create a copy like so:

>     i = list(1,2,3);
>     a = list(i);
>     i = list(5,6,7);
>     print(a);
>     // prints: 1, 2, 3

### Vec3
Vec3 is a simple type intended to bring glm::vec3 into KataScript as a first class type. Since KataScript is designed for game engine integration, a native Vec3 is convenient. Vec3 is created with the `vec3()` function, individual members (x,y,z) are accessed with list acess.

>     v = vec3(1,2,3);
>     print(v[0]);
>     // prints: 1.000000
>     v[1] = 10.0; // currently does not work
>     v = vec3(v[0], 10.0, v[2]); // current way to set members

## Control Flow

### Functions
Functions are called with the syntax `name(arg(s)...)`. For example:

>     print(100);
>     print("hello ", "world");

Functions are created using the `func` keyword. Functions may return values, but it is not strictly required.

>     func add1(a) {
>       return a + 1;
>     }

### Loops
`while()` and `for()` are synonyms that mean start a loop. Either way you can put 1-3 expressions seperated by semicolons inside the parens.
- 1 expression: behaves like a standard while loop
- 2 expressions: behaves like a for loop with no initialization statment
- 3 expressions: behaves like a standard for loop

`foreach(item; collection)` will loop over each item in a list or array with `item` referencing each item 

Then just put the loop contents inside of curly brackets:

>     i = 20;
>     while (i > 0) {
>       print(--i);
>     }

>     for (i = 0; i < 100; i++) {
>       print(i);
>     }

>     foreach (i; [1,2,3] + [4,5]) { print(i); }
>     foreach (i; array(1,2,3,4,5)) { print(i); }
>     foreach (i; someListVariable) { print(i); }

### if/else
`if`/`else if`/`else` all work how you'd expect.

>     if (5 == 5) { 
>       print("pie"); 
>     } else if (5 == 6) { 
>       print("cake"); 
>     } else { 
>       print("coffee"); 
>     }

## Errors
If an error is detected, the expression evaluation will be halted for the current line. Any subexpressions already evaluated are not undone. Error detection is currently basic and many errors will result in undefined behaviour instead.

### Bad Comparison
>     j = 3;
>     print(j > 5);
>     // prints:  0
>     j = "7";
>     print(j > 5);
>     // prints: Error: Bad comparison comparing `STRING 7` to `INT 5`
>     j = 5;
>     print(++j > "5");
>     // prints: Error: Bad comparison comparing `INT 6` to `STRING 5`

### Out of bounds access
>     print(j[0]);
>     // prints: 6
>     print(j[2]);
>     // prints: Error: Out of bounds list access index 2, list length 1

### Quote mistmatch
>     i = "hmmmm;
>     // prints: Error: Quote mismatch at "hmmmm;

### Non-existant function
>     nothing();
>     // prints: Error: Unable to call non-existant function

### Foreach statement count
>     foreach(a) {
>     // prints: Error: Syntax error, `foreach` requires 2 statements, 1 statements supplied instead

### Incorrect token after else
>     if (0) else tornado if (1) {}
>     // prints: Error: Malformed Syntax: Incorrect token `tornado` following `else` keyword

### Array cannot contain collections
>     a = array([1,2],[2,3]);
>     // prints: Error: Array cannot contain collections

----

## Built-in Functions

### Math Operators
`+`/`+=` -> Adds two values, if either is a collection type, it will concatenate them.

`-`/`-=` -> Subtract one number from another, can be used unary to negate a value.

`*`/`*=` -> Multiply two numbbers.

`/`/`/=` -> Divide one number by another.

`%` -> Modulo operation on two numbers.

`++` -> Prefix increment.

`--` -> Prefix decrement.

`=` -> Assign one value to another.

`!` -> True if value is truthy otherwise false.

### Comparison Operators
Note: these all throw an error if you compare a collection to a number.

`==` -> Compare two values for equality, collections are compared element by element, so "hi" == ["h", "i"]

`!=` -> Same as above but opposite result

`&&` -> True if both values are truthy

`||` -> True if neither value is truthy

`<`, `<=`, `>`, `>=` -> Compare less/greater than. Strings are compared lexographically, and lists are compared by length

### Alias Functions
Alias functions are functions that are called by language constructs

`identity(x)` -> Takes x as a reference and then returns that reference. Parenthesis used to denote order of operations use this function to enforce ... order of operations.

`listindex(collection, n)` -> Returns the `n`th element of a collection, throws errors if `n` is out of bounds. Called by square braket index operator.

### Typecast Functions
`int(x)` -> Converts `x` to int

`float(x)` -> Converts `x` to float

`vec3(x, y, z)` -> Create a Vec3 from `x`, `y`, and `z`

`string(x)` -> Converts `x` to string

`array(x...)` -> Create an array from `x` and any other arguments supplied

`list(x...)` -> Create a list from `x` and any other arguments supplied

### Other Functions
`print(s)` -> Prints a string representation of `s` to the console

`typeof(t)` -> Returns a string representing the type of `t`

`getline()` -> Returns a string once it has been entered into the console

`split(str, findstr)` -> Returns an array of strings cut from `str` using `findstr` as boundaries

`sqrt(x)` -> Returns the square root of `x`

`pow(x, n)` -> Returns `x^n`

`length(c)` -> Returns teh size of the collection `c`

`find(c, item)` -> Returns the index in `c` where item exists, or null if no match exists

`contains(c, item)` -> Returns true if the item exists in `c` or false if no match exists

`erase(c, n)` -> Erase the item at index `n` from collection `c`

`range(c, a, b)` -> Returns a new collection from a range from indexes `a` to `b` in collection `c`

`pushback(c, item)` -> Adds `item` to the end of collection `c`

`popback(c)` -> Erases the item at the end of collection `c`

`map(c, f)` -> Applies function `f` to each element of `c` and returns a list of the results

`fold(c, f, initial)` -> Folds function `f` over each element of `c` into `initial` and returns the result

### Precedence
From lowest to highest this is the precedence of operations in KataScript:

Assignment,

Comparison,

Addition/Subtraction,

Multiplicaton/Division/Modulo,

Increment/Decrement,

and finally Parenthesis/Function Calls.


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

### Functional Programming
>     funcs = [print, fib, fizzbuzz];
>     vals = [1,2,3,4,5,6];
>     foreach(v; vals) {
>       print("operating on " + v);
>       foreach(f; funcs) {
>         f(v);
>         print();
>       }
>     }

>     func sub1(n) { return n-1; }
>     foreach(i; map([1,2,3,4], sub1)) { print(i); }

----

## C++ Integration
KataScript is a single header file. To include KataScript into your project simply put KataScript.hpp into your project, `#define KATASCRIPT_IMPL` in exactly `1` cpp file, and then #include it wherever you need.

>     #define KATASCRIPT_IMPL
>     #include "../../KataScript/src/Library/KataScript.hpp"

### Invoke C++ From KataScript
If we want to call C++ code from inside the script, we can register the code as a function any time after calling the KataScriptInterpreter constructor; (note that the function will be placed in the current scope)

Here's a function to wrap for the example: (in C++)
>     int integrationExample(int a) {
>       return a * a;
>     }

Then here's how we register it for use inside of KataScript: (note, this will overwrite any existing function with the same name, so you can use that to redirect the print function for example)
>     KataScript::KataScriptInterpreter interp;
>     auto newfunc = interp.newFunction("integrationExample", [](const KataScript::KSList& args) {
>       // KataScript doesn't enforce argument counts, so make sure you have enough
>       if (args.size() < 1) {
>         return std::make_shared<KataScript::KSValue>();
>       }
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
We can directly call a KataScript function from C++ using the value returned by newFunction().
>     auto varRef = interp.callFunction(newfunc, 4);

We can also send commands from C++ into KataScript using the readLine or evaluate functions.
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

### C++ Types and Methods
All KataScript C++ types are in the KataScript namespace. KataScript uses std::shared_ptr to count references, so functions will generally return a shared pointer to the actuall value. Any time you see a type like `KSThingRef` that means it's an alias for `shared_ptr<KSThing>`

[enum KSType](https://github.com/brwhale/KataScript/blob/main/src/Library/KataScript.hpp#L59) -> This is our type flag. Options are `NONE`, `INT`, `FLOAT`, `FUNCTION`, `STRING`, and `LIST`.

[struct KSValue](https://github.com/brwhale/KataScript/blob/main/src/Library/KataScript.hpp#L405) -> This struct represents a boxed value. If you pull data out of the KataScript environment it will be wrapped in this type. Uses an std::variant to store the actual value so you can use the visitor pattern if you want.
* string getPrintString() -> Get a string representing what printing this value would print
* int& getInt() -> Gets a reference to the internal value as an int
* float& getFloat() -> Gets a reference to the internal value as a float
* KSFunctionRef& getFunction() -> Gets a reference to the internal value as a function reference
* string& getString() -> Gets a reference to the internal value as a string
* KSList& getList() -> Gets a reference to the internal value as a list
All KataScript math operations are implemented by overloading C++ operators on KSValues, so math operations on KSValues in C++ will produce the same results as those operations within KataScript

[alias KSList](https://github.com/brwhale/KataScript/blob/main/src/Library/KataScript.hpp#L288) -> A KSList is just an std::vector of std::shared_ptr to KSValue. This is the data backing for the List type as well as defining the format for function arguments

[alias KSLambda](https://github.com/brwhale/KataScript/blob/main/src/Library/KataScript.hpp#L1284) -> This this the function signature of all KataScript functions. It's an std::function that takes in a const reference to a KSList and returns a shared_ptr to a KSValue.

[class KataScriptInterpreter](https://github.com/brwhale/KataScript/blob/main/src/Library/KataScript.hpp#L1541) -> This is the main class and represents a fully contained KataScript environment. It is instantiated through the default constructor, and will RAII itself with a default destructor. You can run as many instances as you want side by side with no issues.
* KSFunctionRef& newFunction(const string& name, const KSLambda& lam) -> Adds or overrides a C++ lambda as a KataScript function and returns a reference to that function
* KSValueRef callFunction(const KSFunctionRef fnc, ...) -> Call a KataScript function. Arguments are a reference to a KataScript function, and then as many arguments to the function as necessary.
* KSFunctionRef resolveFunction(const string& name) -> Get a reference to a KataScript function by name
* KSValueRef& resolveVariable(const string& name) -> Retrieve a KataScript value by variable name
* void readLine(const string& text) -> Evaluate a line of text as KataScript
* void evaluate(const string& script) -> Evaluate a multi-line KataScript

### C++ Usage Pattern
Using the methods of KataScriptInterpreter, we have a simple pattern for embeded scripting:
1. Initialize a KataScriptInterpreter
2. Register any native functions you want by wrapping them in a KSLambda and submitting that to newFunction()
3. Read your scripts into strings and evaluate() or readLine() them into the KataScriptInterpreter to set up functions and inital state
4. Call into KataScript with callFunction() (or readLine()/evaluate() followed by a resolveVariable()) whenever you want to run KataScript functions.

----
