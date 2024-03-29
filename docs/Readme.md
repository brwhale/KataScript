--- 
layout: page
title: "Readme"
filename: Readme.md
--- 

<style>
    body{color:rgb(208, 208, 208);}
    a{color:rgb(121, 174, 243);}
</style>

# KataScript
KataScript is a simple scripting language with familiar syntax, designed to be easily embedded in C++ applications.

[In-Browser Demo](https://brwhale.github.io/KataScript/)

## Index
- [Overview](https://brwhale.github.io/KataScript/Readme#overview)
  - [Purpose](https://brwhale.github.io/KataScript/Readme#purpose)
  - [Values and Types](https://brwhale.github.io/KataScript/Readme#values-and-types)
  - [Type Coercion](https://brwhale.github.io/KataScript/Readme#typecoercion)
  - [Variables](https://brwhale.github.io/KataScript/Readme#variables)
  - [Memory](https://brwhale.github.io/KataScript/Readme#memory)
  - [Special Characters](https://brwhale.github.io/KataScript/Readme#special-characters)
  - [Keywords](https://brwhale.github.io/KataScript/Readme#keywords)
  - [Comments](https://brwhale.github.io/KataScript/Readme#comments)
- [Control Flow](https://brwhale.github.io/KataScript/Readme#control-flow)
  - [Functions](https://brwhale.github.io/KataScript/Readme#functions)
  - [Loops](https://brwhale.github.io/KataScript/Readme#loops)
  - [if/else](https://brwhale.github.io/KataScript/Readme#ifelse)
- [Collections](https://brwhale.github.io/KataScript/Readme#collections)
  - [Arrays](https://brwhale.github.io/KataScript/Readme#arrays)
  - [Lists](https://brwhale.github.io/KataScript/Readme#lists)
  - [Dictionaries](https://brwhale.github.io/KataScript/Readme#dictionaries)
  - [Vec3](https://brwhale.github.io/KataScript/Readme#vec3)
- [Classes](https://brwhale.github.io/KataScript/Readmed#classes)
  - [Inheritance](https://brwhale.github.io/KataScript/Readme#inheritance)
- [Errors](https://brwhale.github.io/KataScript/Readme#errors)
- [Built-in Functions](https://brwhale.github.io/KataScript/Readme#built-in-functions)
  - [Math Operators](https://brwhale.github.io/KataScript/Readme#math-operators)
  - [Comparison Operators](https://brwhale.github.io/KataScript/Readme#comparison-operators)
  - [Alias Functions](https://brwhale.github.io/KataScript/Readme#alias-functions)
  - [Other Functions](https://brwhale.github.io/KataScript/Readme#other-functions)
  - [Precedence](https://brwhale.github.io/KataScript/Readme#precedence)
- [Examples](https://brwhale.github.io/KataScript/Readme#examples)
  - [Hello World](https://brwhale.github.io/KataScript/Readme#hello-world)
  - [Fizzbuzz](https://brwhale.github.io/KataScript/Readme#fizzbuzz)
  - [Fizzbuzz With Map](https://brwhale.github.io/KataScript/Readme#fizzbuzz-with-map)
  - [Fibonacci Series](https://brwhale.github.io/KataScript/Readme#the-fibonacci-series)
  - [Functional Programming](https://brwhale.github.io/KataScript/Readme#functional-programming)
- [C++ Integration](https://brwhale.github.io/KataScript/Readme#c-integration)
  - [Invoke C++ From KataScript](https://brwhale.github.io/KataScript/Readme#invoke-c-from-katascript)
  - [Invoke KataScript From C++](https://brwhale.github.io/KataScript/Readme#invoke-katascript-from-c)
  - [C++ Types and Methods](https://brwhale.github.io/KataScript/Readme#c-types-and-methods)
  - [C++ Usage Pattern](https://brwhale.github.io/KataScript/Readme#c-usage-pattern)
- [Future Features Roadmap](https://brwhale.github.io/KataScript/Readme#future-features-roadmap)

## Overview

### Purpose
KataScript is designed to be lightweight, secure, sane, and easy to use. I made KataScript because I wanted scripting for my game engine but I did not enjoy the quirks of Lua or the security issues of including a Python or JavaScript environment. Therefore the goal of KataScript is to provide the sanest possible embedded scripting with the least amount of hurdles.

### Values and Types
Like most scripting languages, KataScript is dynamically typed. 

Values can currently have 11 different types: `null`, `int`, `float`, `vec3`, `function`, `userpointer`, `string`, `array`, `list`, `dictionary`, and `class`. 

`null`: the default type of any values. Converts to 0

`int`: 32/64 bit signed integers supplied by underlying C++ int type. Boolean logic uses the int type like C, 0 = False, anything else = True. Number types are 64 bits by default, but you can define `KATASCRIPT_USE_32_BIT_NUMBERS` to use 32 bit numbers.

`float`: 32/64 bit signed floating point IEEE-754 supplied by underlying C++ float type.

`vec3`: 3 32 bit floats wraped into a struct. Used for 3d vectors, math, etc. Directly casts to glm::vec3

`function`: A function reference, can be assigned, called, and passed around but not compared or converted.

`userpointer`: A void* pointer. Similar to a function, a userpointer can be passed around or stored in collections, but not compared or converted.

`string`: A mutable UTF8 character string supplied by underlying C++ std::string type. (some unicode doesn't work in the demo though because of the JavaScript interface)

`array`: A collection of other values. An array must be homogeneous (all values of the same type) and cannot contain other collections, supplied by underlying C++ std::vector containing values contiguously and represented in the unboxed base type. Array data is acessed by integer index starting from 0.

`list`: A collection of other values. A list can be heterogeneous and contain any type, supplied by underlying C++ std::vector containing references to values. List data is acessed by integer index starting from 0. Slower than an `array` but more flexible.

`dictionary`: A collection of other values, but this time as a hashmap. A dictionary can contain any type like a list, but it can be indexed with any non-collection type. Supplied by underlying C++ std::unordered_map type.

`class`: A class type. Contains member variables and functions. Class functions are implicitly called from "class scope" so that means that to each of the class's functions, the class's variables are local. (aka normal class-function scoping compared to other languages)

### Type Coercion
There is minimal type coercion in KataScipt. Int will be promoted to Float for any operation between the two types, and String will be promoted to a List of characters when operating with a List. Attempting to compare a number type to a string or a list (unless it's only 1 element long and that element is a number) will throw an error. If you want to purposely convert values, there are built in casting/parsing functions `bool()`, `int()`, `float()`, `string()`, `array()`, `list()`.

### Variables
Simply attempt to use a variable and it will be created in the current scope.

```c#
i = 5;
```

The variable `i` now stores the integer `5`.

If you want to store a float or a string in `i`, you can do that too.

```c#
i = 5.0; // i is a float
i = "string value"; // i is now a string
```

If you run into scoping issues, you can also use the `var` keyword to ensure a new variable is created. Variables created inside functions should use the var keyword to avoid issues when using higher order functions.

```c#
var i = 3;
```

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
```c#
// this is a comment
```
Comments need not start at the beginnning of a line.

## Control Flow

### Functions
Functions are called with the syntax `name(arg(s)...)`. For example:

```c#
print(100);
print("hello ", "world");
```

Functions can also be called using dot syntax:

```c#
// these are the same
length([1,2,3,4]);
[1,2,3,4].length();

// these too
range([1,2,3,4],2,3);
[1,2,3,4].range(2,3);
```

Functions are created using the `func` keyword (or `fn` or `function` if you want). Functions may return values, but it is not strictly required.

```c#
func add1(a) {
  return a + 1;
}
```

### Loops
`while()` and `for()` are synonyms that mean start a loop. Either way you can put 1-3 expressions seperated by semicolons inside the parens.
- 1 expression: behaves like a standard while loop
- 2 expressions: behaves like a for loop with no initialization statment
- 3 expressions: behaves like a standard for loop

`foreach(item; collection)` will loop over each item in a list or array with `item` referencing each item 

Then just put the loop contents inside of curly brackets:

```c#
i = 20;
while (i > 0) {
  print(--i);
}
```

```c#
for (i = 0; i < 100; i++) {
  print(i);
}
```

```c#
foreach (i; [1,2,3] + [4,5]) { print(i); }
foreach (i; array(1,2,3,4,5)) { print(i); }
foreach (i; someListVariable) { print(i); }
```

### if/else
`if`/`else if`/`else` all work how you'd expect.

```c#
if (5 == 5) { 
  print("pie"); 
} else if (5 == 6) { 
  print("cake"); 
} else { 
  print("coffee"); 
}
```

## Collections
A collection literal looks like `[value(s)...]` and a collection access looks like `name[index]` (note that collections are 0-indexed). A collection literal will resolve to an `array` if all the types are the same and a `list` otherwise.
The `length()` function can tell you the size of a collection.

### Arrays
An array is a collection of unboxed values. All values in an array must be the same type and that type cannot be a collection.

```c#
i = [1, 2, 3]; // array from literal
i = array(1, "fish", 2, 3); // array function
// array type is the type of the first element added.
print(i[1]);
// prints: 2, note how "fish" was not added to the array
print(length(i));
// prints: 3, again only the integer values were accepted
```

### Lists
A list is a collection of boxed value references. A list can store values of any type and can store other collections.

```c#
i = [1.0, 2, 3, "squid"]; // list from literal
i = list(1, 2, 3); // list function
print(i[1]);
// prints: 2
print(length(i));
// prints: 3
j = list(4, 5, 6);
k = list(7, 8, 9);
a = list(i, j, k);
print(a);
// prints: 1, 2, 3, 4, 5, 6, 7, 8, 9
i = list("fish", "tacos");
print(a);
// prints: fish, tacos, 4, 5, 6, 7, 8, 9
k = list(7, "string", 9.0);
print(a);
// prints: fish, tacos, 4, 5, 6, 7, string, 9.000000
```

Note how list elements follow reference semantics, if you wish to actually copy the data into a list, you can use the copy function to create a copy like so:

```c#
i = list(1,2,3);
a = copy(i);
i = list(5,6,7);
print(a);
// prints: 1, 2, 3
```

### Dictionaries
A dictionary is a collection that can be indexed by any non-collection type

```c#
i = dictionary();
i["squid"] = 10;
i["octopus"] = 8;
i[69] = "nice";
i[i[69]] = "this is in i[\"nice\"]";
```

### Vec3
Vec3 is a simple type intended to bring glm::vec3 into KataScript as a first class type. Since KataScript is designed for game engine integration, a native Vec3 is convenient. Vec3 is created with the `vec3()` function, individual members (x,y,z) are accessed with list acess.

```c#
v = vec3(1,2,3);
print(v[0]);
// prints: 1.000000
v[1] = 10.0; // currently does not work
v = vec3(v[0], 10.0, v[2]); // current way to set members
```

## Classes
A class is a multiply inheritable object. Declare a class with the class keyword.

In order to use a class, you need a `constructor`, which is the function to create and return instances of your struct. To create a constructor, simply create a function with the same name as the struct that it's in. The constructor can take any number of arguments, and returning the instance is automatically handled by the language runtime, so all you need to do is set the state of the object and you're good to go.

```c#
class person {
    var name;
    var age;
    var hobby;
    func person(n, a, h) {
        name = n;
        age = a;
        hobby = h;
    }
    func wouldLike(other) {
        return hobby == other.hobby;
    }
    func greet() {
        print("Hey I'm " + name + ", age " + age + ", and my hobby is " + hobby);
    }
}
var me = person("Garrett", 28, "programming");
me.greet();
// prints: Hey I'm Garrett, age 28, and my hobby is programming
print(me.wouldLike(person("You", 0, "programming")));
// prints: 1 (aka true)
```

### Inheritance
A class can inherit from any number of other classes. The official inheritance operator is `->` but you can use any token or series of tokens you want. When inheriting from multiple parent classes, sperate the names with `,`
```c#
class xx { 
    var x; 
    func xx(_x) { 
        x = _x; 
    } 
    func add(_x, _y) { 
        x += _x; y += _y; 
    } 
}
class yy { 
    var y; 
    func yy(_y) { 
        y = _y; 
    } 
    func sqr() { 
        return x * y; 
    } 
}
// You get the functions and variables of each parent class (but not the constructors)
// in event of name collisions, last one in wins
class prexy -> xx, yy { 
    func prexy(_x, _y) { 
        x = _x; y = _y; 
    } 
}
// You get the inherited parents too
class xy -> prexy { 
    func xy(_x, _y) { 
        x = _x; y = _y;
    } 
}
a = xy(4,5.0); 
b = copy(a); 
b.add("a","b"); 
c = a.sqr(); 
a.add(4,5); 
d = a.sqr(); 
e = a.x; 
f = a.y;
print(a);
// prints:
// xy:
// `x: 8`
// `y: 10.000000`
// `add: add`
// `sqr: sqr`
print(b);
// prints:
// xy:
// `x: 4a`
// `add: add`
// `y: 5.000000b`
// `sqr: sqr`
print(c);
// prints: 20.000000
print(d);
// prints: 80.000000
print(e);
// prints: 8
print(f);
// prints: 10.000000
```

## Errors
If an error is detected, evaluation will be halted (for the current line in interpereted mode). Error detection is currently basic and some errors will result in undefined behaviour instead.

### Bad Comparison
```c#
j = 3;
print(j > 5);
// prints:  0
j = "7";
print(j > 5);
// prints: Error: Bad comparison comparing `string 7` to `int 5`
j = 5;
print(++j > "5");
// prints: Error: Bad comparison comparing `int 6` to `string 5`
```

### Out of bounds access
```c#
print(j[0]);
// prints: 6
print(j[2]);
// prints: Error: Out of bounds list access index 2, list length 1
```

### Quote mistmatch
```c#
i = "hmmmm;
// prints: Error: Quote mismatch at "hmmmm;
```

### Non-existant function
```c#
nothing();
// prints: Error: Unable to call non-existant function
```

### Foreach statement count
```c#
foreach(a) {
// prints: Error: Syntax error, `foreach` requires 2 statements, 1 statements supplied instead
```

### Incorrect token after else
```c#
if (0) else tornado if (1) {}
// prints: Error: Malformed Syntax: Incorrect token `tornado` following `else` keyword
```

### Array cannot contain collections
```c#
a = array([1,2],[2,3]);
// prints: Error: Array cannot contain collections
```

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

`!` -> Prefix Boolean Not
 
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

`listindex(collection, n)` -> Returns the `n`th element of a collection, throws errors if `n` is out of bounds, converts the collection to a dictionary if `n` is not an int. Called by square braket index operator.

`applyfunction(name/func, {class}, ...)` -> Applies the function (by string name or function value) to the arguments. If the first arg is a class, then it will look for a member function on the class before looking for a standard free function.

### Typecast Functions
`int(x)` -> Converts `x` to int

`float(x)` -> Converts `x` to float

`vec3(x, y, z)` -> Create a Vec3 from `x`, `y`, and `z`

`string(x)` -> Converts `x` to string

`array(x...)` -> Create an array containing `x` and any other arguments supplied

`list(x...)` -> Create a list containing `x` and any other arguments supplied

`toarray(x)` -> Converts `x` to array

`tolist(x)` -> Converts `x` to list

`dictionary(x)` -> Create a dictionary using collection `x` as a source

### Other Functions
`print(s)` -> Prints a string representation of `s` to the console

`typeof(t)` -> Returns a string representing the type of `t`

`copy(x)` -> Create a copy of `x`

`getline()` -> Returns a string once it has been entered into the console

`split(str, findstr)` -> Returns an array of strings cut from `str` using `findstr` as boundaries

`sqrt(x)` -> Returns the square root of `x`

`pow(x, n)` -> Returns `x^n`

`sin(x)` -> Returns sine of `x`

`cos(x)` -> Returns cosine of `x`

`tan(x)` -> Returns tangent of `x`

`clock()` -> Returns the current time in nanoseconds since the epoch as an int

`timesince(x)` -> Returns the time since `x` as a float of seconds where `x` is an int of nanoseconds since the epoch

`getduration(a,b)` -> Returns the time between `a` and `b` as a float of seconds where `a` and `b` are ints of nanoseconds since the epoch

`length(c)` -> Returns teh size of the collection `c`

`find(c, item)` -> Returns the index in `c` where item exists, or null if no match exists

`contains(c, item)` -> Returns true if the item exists in `c` or false if no match exists

`erase(c, n)` -> Erase the item at index `n` from collection `c`

`range(c, a, b)` -> Returns a new collection from a range from indexes `a` to `b` in collection `c`

`range(a, b)` -> Returns a new array with int or float values ranging from `a` to `b`. `b` can be larger or smaller than `a`.

`pushback(c, item)` -> Adds `item` to the end of collection `c`

`popback(c)` -> Erases the item at the end of collection `c`

`sort(c)` -> Sorts collection `c`

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
```c#
print("hello world");
```

### Fizzbuzz
```c#
func fizzbuzz(n) {
  for(i=1;i<=n;i++) { 
    if (i % 15 == 0) { 
      print("fizzbuzz"); 
    } else if (i % 3 == 0) { 
      print("fizz"); 
    } else if (i % 5 == 0) { 
      print("buzz"); 
    } else { 
      print(i);
    } 
  }
}
```

### Fizzbuzz With Map
```c#
func fizz(n) {
    if (n % 15 == 0) {
        return "FizzBuzz";
    } else if (n % 3 == 0) {
        return "Fizz";
    } else if (n % 5 == 0) {
        return "Buzz";
    }
    return n;
}
func fizzbuzz(n) {
    foreach(result; range(1,n).map(fizz)) {
        print(result);
    }
}
```

### The Fibonacci Series
```c#
// print all Fibonacci numbers up to c
func printfibs(c) {
  i = 0;
  j = 1;
  while(i<c) { 
    print(i); 
    i += j; 
    swap(i, j);
  }
}
// recursively find Fibonacci number at index n:
func fib(n) {
    if (n < 2) {
        return n;
    }
    return fib(n-1) + fib(n-2);
}
```

### Functional Programming
```c#
funcs = [print, printfibs, fizzbuzz];
vals = [1,2,3,4,5,6];
foreach(v; vals) {
  print("operating on " + v);
  foreach(f; funcs) {
    f(v);
    print();
  }
}
```

```c#
func sub1(n) { 
  return n-1; 
}
foreach(i; map([1,2,3,4], sub1)) { 
  print(i); 
}
```

----

## C++ Integration
KataScript is a single header file. To include KataScript into your project simply put KataScript.hpp into your project, `#define KATASCRIPT_IMPL` in exactly `1` cpp file, and then #include it wherever you need. Note that KataScript's implementation uses C++20, so you need g++9/MSVC19 or higher to compile it.

```c++
#define KATASCRIPT_IMPL
#include "../../KataScript/src/Library/KataScript.hpp"
```

### Invoke C++ From KataScript
If we want to call C++ code from inside the script, we can register the code as a function any time after calling the KataScriptInterpreter constructor; (note that the function will be placed in the current scope)

Here's a function to wrap for the example: (in C++)
```c++
int integrationExample(int a) {
  return a * a;
}
```

Then here's how we register it for use inside of KataScript: (note, this will overwrite any existing function with the same name, so you can use that to redirect the print function for example)
```c++
KataScript::KataScriptInterpreter interp;
auto newfunc = interp.newFunction("integrationExample", [](const KataScript::List& args) {
  // KataScript doesn't enforce argument counts, so make sure you have enough
  if (args.size() < 1) {
    return std::make_shared<KataScript::Value>();
  }
  // Dereference argument
  auto val = *args[0];
  // Coerce type
  val.hardconvert(KataScript::Type::Int);
  // Call c++ code
  auto result = integrationExample(val.getInt());
  // Wrap and return
  return std::make_shared<KataScript::Value>(result);
});
```

Now we can call that function from inside our scripts.

### Invoke KataScript From C++
We can directly call a KataScript function from C++ using the value returned by newFunction().
```c++
auto varRef = interp.callFunction(newfunc, 4);
```

We can also send commands from C++ into KataScript using the readLine or evaluate functions.
```c++
interp.readLine("i = integrationExample(4);");
```

Now `i` contains `16` and is an int.
If we want to pull that value out, we can do that too!

```c++
auto varRef = interp.resolveVariable("i");
 
// visit style
std::visit([](auto&& arg) {std::cout << arg; }, varRef->value);
   
// if the type is known
int i = varRef->getInt();
  
// switch style
switch (varRef->type) {
case KataScript::Type::Int:
  std::cout << varRef->getInt();
  break;
case KataScript::Type::Float:
  std::cout << varRef->getFloat();
  break;
case KataScript::Type::String:
  std::cout << varRef->getString();
  break;
}
```

### C++ Types and Methods
All KataScript C++ types are in the KataScript namespace. KataScript uses std::shared_ptr to count references, so functions will generally return a shared pointer to the actuall value. Any time you see a type like `ThingRef` that means it's an alias for `shared_ptr<Thing>`

enum `Type -> This is our type flag. Options are `Null`, `Int`, `Float`, `Vec3`, `Function`, `UserPointer`, `String`, `Array`, `List`, `Dictionary`, and `Class`

struct Value -> This struct represents a boxed value. If you pull data out of the KataScript environment it will be wrapped in this type. Uses an std::variant to store the actual value so you can use the visitor pattern if you want.
* string getPrintString() -> Get a string representing what printing this value would print
* int& getInt() -> Gets a reference to the internal value as an int
* float& getFloat() -> Gets a reference to the internal value as a float
* FunctionRef& getFunction() -> Gets a reference to the internal value as a function reference
* string& getString() -> Gets a reference to the internal value as a string
* List& getList() -> Gets a reference to the internal value as a list
* Dictionary& getDictionary() -> Gets a reference to the internal value as a dictionary
All KataScript math operations are implemented by overloading C++ operators on Values, so math operations on Values in C++ will produce the same results as those operations within KataScript

alias KataScript::List -> A List is just an std::vector of std::shared_ptr to KataScript::Value. This is the data backing for the List type as well as defining the format for function arguments

alias KataScript::Dictionary -> A Dictionary is just an std::unordered_map<size_t, std::shared_ptr<Value>>. This is the data backing for the Dictionary type, using 64 bit hashes (size_t) as a key

alias KataScript::Lambda -> This this the function signature of all KataScript functions. It's an std::function that takes in a const reference to a KataScript::List and returns a shared_ptr to a KataScript::Value

class KataScriptInterpreter -> This is the main class and represents a fully contained KataScript environment. It is instantiated through the default constructor, and will RAII itself with a default destructor. You can run as many instances as you want side by side with no issues.
* FunctionRef& newFunction(const string& name, const Lambda& lam) -> Adds or overrides a C++ lambda as a KataScript function and returns a reference to that function
* ValueRef callFunction(const FunctionRef fnc, ...) -> Call a KataScript function. Arguments are a reference to a KataScript function, and then as many arguments to the function as necessary.
* FunctionRef resolveFunction(const string& name) -> Get a reference to a KataScript function by name
* ValueRef& resolveVariable(const string& name) -> Retrieve a KataScript value by variable name
* void readLine(const string& text) -> Evaluate a line of text as KataScript
* void evaluate(const string& script) -> Evaluate a multi-line KataScript

### C++ Usage Pattern
Using the methods of KataScriptInterpreter, we have a simple pattern for embeded scripting:
1. Initialize a KataScriptInterpreter
2. Register any native functions you want by wrapping them in a Lambda and submit that to newFunction()
3. Read your scripts into strings and evaluate() or readLine() them into the KataScriptInterpreter to set up functions and inital state
4. Call into KataScript with callFunction() (or readLine()/evaluate() followed by a resolveVariable()) whenever you want to run KataScript functions.

----

## Future Features Roadmap
These are things that are planned additions:
- Near term
* add more modules
- Mid term
* async/threading
- Long term
* Integrate with llvm to produce executables
