# RPN Shell / Stack-Based Scripting language
A stack based script (.sbs) interpreter which is almost a scripting language at this point. Using the shell, it can be used as a calculator by those familiar with the notation, and it should be relatively useful by the time I'm finished.

# Build and Run
If you are a windows user and don't know anything about C++ compilers, you can download this precompiled executable (<b>warning:</b> it's old). <b>Warning</b>, I cross-compiled this on Linux and have only run it in a virtual machine, so it might not be perfect.<br/> <h6><pre>i686-w64-mingw32-g++ *.cpp --static -Wall -o sbs.exe</pre></h6>
https://1drv.ms/u/s!AqWtCxMOBjWMhbc0ZRkaWosjm5IWXw
- Compiling:
`rpn $ g++ *.cpp -o rpn`
- Running:
`rpn $ ./rpn`

# How to use
 - <b>Comments:</b>
   Anything after the `#` symbol will be ignored.
   ```
   >>> 32 # this is a comment
   32
   ```
   
 - <b>Binary operators:</b>
  Place the operator after it's two operands.
   ```
   >>> 1 2 +    # 1 + 2 
   3
   >>> 2 5 **   # 2 ^ (5)
   32
   ```
   
 - <b>Unary operators:</b>
  place the unary operator/funciton behind it's parameter
  ```
  >>> 100 log   # log10(100)
  2
  >>> 0.5 sin   # sine of 90 radians
  0.479426
  ```
  
 - <b>Combining operators:</b>
  One of RPN's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performed sequentially.
  ```
  >>> 1 1 + 2 *    # (1 + 1) * 2  
  4
  ```
 - <b>Variables:</b>
  This calculator supports user-defined variables created as soon as you start using them. All variable names must start with a `$` to prevent them from clashing with other keywords, functions and numbers. Here's how to define one:
 ```
 >>> $a 4 =
 >>> $a print
 4
 ```
  Variables are references to data, variables can reference eachother. Changing the value of a reference changes the value of it's referenced variable.  If you want to make a deep copy, use the copy operator (`~`). 
 ```
 >>> $b $a ~ = # now they both hold the same value
 ```
 - <b>Strings:</b>
  This is a loosely-typed language. Strings are enclosed in double quotes `"` and only need a closing `"` if it would change the meaning without it.
 ```
 >>> "closing quote not needed here
 "closing quote not needed here
 "
 
 >>> "hello " "there" + $a = # notice that the `+` operator is overloaded
 "hello there"
 
 >>> $a print 
 hello there
 ```
 - <b>Conditionals:</b>
  Sometimes code should only run under certain conditions. Conditionals are initiated after a boolean with a `?:` and are ended with a `:?`. An empty conditional can also be used as a multi-line comment. Currently conditionals aren't fully implemented.
```
>>> # prompt the user for the day of the week and assign it to $day
>>> "What day of the week is it?" print     $day input =  
What day of the week is it? tuesday
ans "tuesday" =
>>> $day "monday" == ?:                 # if today is monday 
    "I hate mondays...\n" print         # so true
:? $day "friday" == ?:                  # else if today is friday
    "Gotta get down on friday!\n" print # I hate that song
:? ?:                                   # else
    "Thanks!\n" print 
:?
Thanks!
>>>
```
 
 - <b>Keywords and Commands:</b>
    * `>>> help` <br/>
      The help command displays a short dialog to help get you started.
    *`>>> clear` <br/>
      clears the terminal (using a system call (suports 3 major operating systems))
    * `>>> reset` <br/>
      resets the line numbering and variables
    * `>>> exit` <br/>
      closes the program
    * `>>> q` <br/>
      same as `exit`
    * `>>> ans` <br/>
      previous answer, initialized to 0. (access it as you would a variable)
    * `>>> pi` <br/>
      an approximate value for pi (access it as you would `ans`)
    * `>>> true` <br/>
      puts 1 on the top of the stack
    * `>>> false` <br/>
      puts 0 on the top of the stack
    * `>>> null` <br/>
      puts a NULL object on the top of the stack (depricated)
    * `>>> print` <br/>
      prints the element at the top of the stack
    * `>>> println` <br/>
      prints the element at the top of the stack and a newline
    * `>>> input` <br/>
      returns user input as a string
    * `>>> vars` = `>>> listvars` <br/>
      prints the status and types of all the variables (useful for debugging)
    * `>>> typeof`<br/>
      returns the type of the given variable or constant
    * `>>> syscall`<br/>
      calls `system()` on the given string.
    * `>>> errors-on` & `>>> errors-off`<br/>
      enable/disable errors (by default they are on)


# A incomplete list of built-in operators and functions:
 * Mathematical operators:
  - `+`: add (also concatenates when one or more of the arguments is a string)
  - `-`: subtract
  - `*`: multiply
  - `/`: divide
  - `%`: modulus/remainder
  - `**` = `pow`: raise arg1 to arg2 power
  - `sqrt` = `sqr`: square route
  - `log`, `log10`: base 10 logarithm
  - `ln`: natural logarithm
  - `logbase` = `logBase` = `logBASE`: Logarithm of the given base
  - `sin`, `cos`, `tan`: trigonometric funcitons (radians)
  - `asin`, `acos`, `atan`: inverse trigonometric funcitons (radians)
  - `sinh`, `cosh`, `tanh`: hyperbolic trigonometric functions (radians)
  - `asinh`, `acosh`, `atanh`: inverse hyperbolic trigonometric functions (radians)
  
 * Logical operators: (0 is false everything else is true)
  - `&&`: logical-and
  - `||`: logical-or
  - `!`: logical-not
  - `!=`: logical-XOR
 
 * Compairison operators:
  - `==`: equals-to
  - `!=`: not equals-to
  - `>`: greater than
  - `<`: less than
  - `>=`: greater than or equal to
  - `<=`: less than or equal to
  
 * Bitwise operators:
  - `|`: bitwise-or
  - `&`: bitwise-and
  - `^`: bitwise-XOR
  - `<<`: bitshift left
  - `>>`: bitshift right
 
* Stack/flow operators: these are useful in organizing your syntax
  - `...`: empty stack
  - `;`: pop top of stack
  - `dup`: duplicates the value on the top of the stack (1 arg)
  - `dupn`: performs `dup` n number or times (2 args)
  - `swap`: swaps the top 2 elements in the stack
  
* Type conversions:
  - `str`: converts to string
  - `num`: converts to a number
  - `int`: converts to a integer (rounding)
  - `floor`: static casts to an int. 
  - `~`: copy operator, replaces a reference with the basic value it points to
  
* String functions:
  - `strlen`: returns the length of a given string
