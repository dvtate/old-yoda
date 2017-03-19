# RPN Shell / Yoda-Script
A Yoda-Script (.ys) interpreter which is now turing complete! Using the shell, it can be used as a calculator by those familiar with reverse polish notation (RPN). It is already useful, but it still has a ways to go.

# Build and Run [![Build Status](https://travis-ci.org/dvtate/yoda.svg?branch=master)](https://travis-ci.org/dvtate/yoda/)
If you are a windows user and don't know anything about C++ compilers, you can download this precompiled executable (<b>NOTICE:</b> it's old [2016.12.5]). <b>Warning</b>, I cross-compiled this on Linux and have only run it in a virtual machine, so it might not be perfect.<br/> <!--<h6><pre>i686-w64-mingw32-g++ *.cpp --static -Wall -o sbs.exe</pre></h6>-->
https://1drv.ms/u/s!AqWtCxMOBjWMhqRo9xkCgNeJKjWgNA 
- Compiling:
`yoda $ g++ *.cpp -o yoda`
- Running:
`yoda $ ./yoda`
- Installing:
`yoda $ sudo mv yoda /usr/bin/yoda` <br/>
After installing, you can run yoda as you would any other program from the terminal.
# About YodaScript
YodaScript is a stack-based language based on reverse polish notation.

- Some features which I plan on implementing:
  + functions and subroutines
  + lists
  + loops and if statements that actually work
  + scoping (maybe) (maybe something like what PHP does)
  + proper string manipulation funcitons and such
  + regular expressions
  + concurrency (multi-threaded/async)
  
- Some features I will not implement:
  + OOP (unless implemented by someone else ;) )
  + goto's (essentially impossible given my current design)
  
# How to use (note- out of date)
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
  >>> 0.5 sin   # sine of 0.5 radians 
  0.479426
  ```
  
 - <b>Combining operators:</b>
  One of postfix notation's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performed sequentially.
  ```
  >>> 1 1 + 2 *    # (1 + 1) * 2  
  4
  ```
 - <b>Variables:</b>
  Variables are created as soon as you start using them. All variable names must start with a `$` to prevent them from clashing with other keywords and literals. Here's an example:
```
 >>> $a 4 =
 >>> $a print
 4
```
  Variables are references to data, variables can reference eachother. Changing the value of a reference changes the value  of it's referenced variable.  If you want to make a deep copy, use the copy operator (`~`). 
```
>>> $a 8 =    # now $a is a reference to the number 8 
>>> $b $a =   # now $b is a reference to $a and $a is a reference to the number 8
>>> $b $a ~ = # now they are both references to the number 8
```
 - <b>Strings:</b>
  This is a loosely-typed language. Strings are enclosed in double quotes `"` and only need a closing `"` if it would change the meaning without it. (ie - end-of-line automatically adds a '\n' and ends the quote)
 ```
 >>> "hello " "there" + $a = # notice that the `+` operator is overloaded
 "hello there"
 
 >>> $a print
 hello there
 ```
 - <b>Executable Arrays:</b>
  Depending on the context these are also called anonymous subroutines. They are containers of code and function like most other data, working with operators. You can run the code in one with the `@` operator.
  ```
  >>> $mySub {
    "hello there" print
    $a 5 =
  } =
  >>> $mySub @
  hello there
  >>> $a print # note, variables declared within the subroutine remain after it's done
  5
  ```
  
 - <b>Loops:</b>
  Loops can be used to repeat the same code a number of times. This language has several types of loops built in.
  - <b>Repeat Loops:</b>
   runs code <i>n</i> times
   ```
   >>> { "ha" print } 5 repeat
   hahahahaha
   ```
  - <b>While Loops:</b>
   runs core while a condition is met (checks before each cycle)
   ```
   >>> $count 0 =    # set $count to 0
   >>> {
       "count is " $count + println
       $count $count 1 + =   # increment $count by 1
   } { $count 5 < } while
   count is 0
   count is 1
   count is 2
   count is 3
   count is 4
   ```
 - <b>Conditionals:</b>
  These can be used to run code if a condition is met
  ```
  >>> "Enter GPA: " print $gpa input int =
  Enter GPA: 4
  >>> #this first method is syntactic sugar and slower than the second
  >>> { "You can do better than that." println } else 
  { "That's good enough for me." println } $gpa 3 >= elseif 
  { "Woooooooooooow!" println } $gpa 4 >= if
  Woooooooooooow!
  
  >>> # this method has better performance but gets ugly quick
  >>> {
      { "You can do better than that." println } else 
      { "That's good enough for me." println } $gpa 3 >= if
  } else {
      "Woooooooooooow!" println
  } $gpa 4 >= if
  Woooooooooooow!
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
    * `>>> q`<br/>
      same as `exit`
    * `>>> ans`<br/>
      previous answer, initialized to 0. (access it as you would a variable)
    * `>>> pi`<br/>
      an approximate value for pi (access it as you would `ans`)
    * `>>> true`<br/>
      puts 1 on the top of the stack
    * `>>> false`<br/>
      puts 0 on the top of the stack
    * `>>> null`<br/>
      puts a NULL object on the top of the stack (deprecated)
    * `>>> print`<br/>
      prints the element at the top of the stack
    * `>>> println`<br/>
      prints the element at the top of the stack and a newline
    * `>>> input`<br/>
      returns user input as a string
    * `>>> getchar`<br/>
      inputs a single character from the user as a string
    * `>>> vars` = `>>> listvars` <br/>
      prints the status and types of all the variables (useful for debugging)
    * `>>> typeof`<br/>
      returns the type of the given variable or constant
    * `>>> syscall`<br/>
      calls `system()` on the given string.
    * `>>> errors-on` & `>>> errors-off`<br/>
      enable/disable errors (by default they are on)
    * `>>> color_print`<br/>
      prints a value (string, number, etc.) to the terminal in a given HTML color in a string. The color should be on top
    * `>>> setFgColor` & `>>> setBgColor`<br/>
      changes the terminal color to a given html color contained in a string
    * `>>> reset_color` & `>>> resetFgColor` & `>>> resetBgColor` <br/>
      resets the background and/or foreground colors
    * `>>> file_get_contents`<br/>
      loads the contents of a file into a string
    * `>>> file_put_contents`<br/>
      loads the contents of a string into a file
      
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
  - `==`, `?=`: equals-to
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
  
* Execuatable Array Operators:
  - `@`, `eval`: runs the given subroutine or string as code (equivalent to eval)

* Structure-Equivalent Operators:
  - `else`, `elseif`, `if`: conditional operators
  - `repeat`, `while`: looping operators
