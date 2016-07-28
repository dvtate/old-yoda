# RPN Calculator
A reverse polish notation interpreter which is laying the frameworks for a scripting langauge. It can be used as a calculator by those familiar with the notation, and it should be functional by the time I'm finished. 

# How to use
 - <b>Comments:</b>
   Anything after the `#` symbol will be ignored.
   ```
   >>> 32 # this is a comment
   ans 32 =
   ```
   
 - <b>Binary operators:</b>
  Place the operator after it's two operands.
   ```
   >>> 1 2 +    # 1 + 2 
   ans 3 =
   >>> 2 5 **   # 2 ^ (5)
   ans 32 =
   ```
   
 - <b>Unary operators/funcitons:</b>
  place the unary operator/funciton behind it's parameter
  ```
  >>> 100 log   # log10(100)
  ans 2 =
  >>> 0.5 sin   # sine of 90 radians
  ans 0.479426 =
  ```
  
 - <b>Combining operators:</b>
  One of RPN's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performed sequentially.
  ```
  >>> 1 1 + 2 * # (1 + 1) * 2  
  ans 4 =
  ```
 - <b>Variables:</b>
  This calculator supports user-defined variables created as soon as you start using them. All variable names must start with a `$` to prevent them from clashing with other keywords, functions and numbers. Here's how to define one:
 ```
 >>> $a 4 =
 ans 4 =
 ```
  You can access the variable by using it's name. You can treat variables just as a regular numerical constant, the only difference is you can change it's value. You can even use it to set a variable to another variable's value.
 ```
 >>> $b $a =
 ans 4 =
 ```
 - <b>Strings:</b>
  This is a loosely-typed language. Strings are enclosed in double quotes `"` and only need a closing `"` if it would change the meaning without it.
 ```
 >>> "closing quote not needed here
 ans "closing quote not needed here" =
 
 >>> "hello " "there" + $a = # notice that the `+` operator is overloaded
 ans "hello there" =
 
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
    * `>>> print` <br/>
      prints the element at the top of the stack
    * `>>> input` <br/>
      returns user input as a string
    * `>>> str` <br/>
      converts a number to a string (or passes a string)
    * `>>> num` <br/>
      converts a string to a number (or passes a number)
    * `>>> int` <br/>
      converts a string or a number to an integer.
    * `>>> vars` = `>>> listvars` <br/>
      prints the status of all the variables (useful for debugging)
    * `>>> typeof`<br/>
      returns the type of the given variable or constant
    * `>>> syscall`<br/>
      calls `system()` on the given string.
    * `>>> errors-on` & `>>> errors-off`<br/>
      enable/disable errors (by default they are on)

# Build and Run
If you are a windows user and don't know anything about C++ compilers, you can download this precompiled executable (<b>warning:</b> it's old). <b>Warning</b>, I cross-compiled this on Linux and haven't tested it on a genuine windows box yet, so there could be errors.<br/> <h6><pre>i686-w64-mingw32-g++ *.cpp --static -Wall -o rpn.exe</pre></h6>
<i>Windows binary removed because it is obsolete (will make another later)</i>
- Compiling:
`rpn $ g++ *.cpp -o rpn`
- Running:
`rpn $ ./rpn`



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
  - `sin`,`cos`,`tan`: trigonometric funcitons (radians)
  - `asin`,`acos`,`atan`: inverse trigonometric funcitons (radians)
  - `sinh`,`cosh`,`tanh`: hyperbolic trigonometric functions (radians)
  - `asinh`,`acosh`,`atanh`: inverse hyperbolic trigonometric functions (radians)
  
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
  - `~`: bitwise-not
  - `<<`: bitshift left
  - `>>`: bitshift right
 
* Stack operators:
  - `...`: empty stack
  - `!!`: pop top of stack
