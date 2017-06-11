# RPN Shell / Yoda-Script
A Yoda-Script (.ys) interpreter which is now turing complete! The language is dynamically typed, and although it has a rather unique syntax, those familiar to RPN may prefer it to other languages. This is just a side project of mine suggestions and help are welcome. Using the shell, it can be used as a calculator by those familiar with reverse polish notation (RPN). It is already useful, but it still has a ways to go to become a practical language for develolpers.

# Build and Run [![Build Status](https://travis-ci.org/dvtate/yoda.svg?branch=master)](https://travis-ci.org/dvtate/yoda/)
If you are a Windows user and don't know anything about C++ compilers, you can download this precompiled executable  <b>Warning</b>, I cross-compiled this on Linux, it hasn't been tested on a native installation<br/>
http://dvtate.github.io/dls/yoda.exe [updated 2017.6.4]
- Compiling:
`yoda $ make`
- Running:
`yoda $ ./yoda`
- Installing:
`yoda $ sudo make install`
- Uninstalling:
`yoda $ sudo make uninstall`

After installing, you can run yoda as you would any other program from the terminal.

# About YodaScript
YodaScript is a stack-based language based on reverse polish notation.
- Features coming soon (fast-track):
  + lists (std::vector<CalcValue>)

- Some features which I plan on implementing (totem-poll):
  + **proper** functions and subroutines (not macros)
  + associative arrays (dictionaries)
  + proper string manipulation funcitons and such
  + regular expressions
  + concurrency (multi-threaded/async) (maybe with std::thread)

- Some features I do not plan on implementing:
  + OOP (unless implemented by someone else ;) )
  + goto's (essentially impossible given my current design)

# How to use (note- may be out of date)
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
  Variables are references to data, variables can reference eachother. Changing the value of a reference changes the value  of it's referenced variable.  If you want to make a deep copy, use the copy operator (`~`). Also, note: this language uses dynamic scope resolution and variables are deleted as they go out of scope. If 2 variables in different scopes have the same name, the one in the more specific scope will be used.
```
>>> $a 8 =    # now $a is a reference to the number 8
>>> $b $a =   # now $b is a reference to $a and $a is a reference to the number 8
>>> $b $a ~ = # now they are both references to the number 8
```
 - <b>Strings:</b>
  This is a loosely-typed language. Strings are enclosed in double quotes `"` and only need a closing `"` if it would change the meaning without it. (ie - end-of-line automatically adds a '\n' and ends the quote)
 ```
 >>> "hello " "there" + $a = # notice that the `+` operator is overloaded
 >>> $a print
 hello there
 ```
 - <b>Macros:</b>
  Depending on the context these are also called anonymous subroutines. They are containers of code and function like most other data, working with operators. You can run the code in one with the `@` operator.
  ```
  >>> $mySub {
    "hello there" print
    $a 5 =
  } =
  >>> $mySub @
  hello there
  >>> $a print # note, variables declared within the macro are deleted wen they go out of scope
  ERROR: broken reference to `$a`.
  ```

 - <b>Loops:</b>
  Loops can be used to repeat the code a number of times. This language has several types of loops built in.
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
  - <b>for-each Loops:</b>
   assigns a variable to each element in an execuatable array
   ```
   1>>> {
	     $num print  
   } { 4 3 2 1 0 } $num for-each
   01234
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
  - <b>Lists:</b>
    lists hold multiple pieces of data in one container. They contain diverse types and are handled like any other data.
    ```
    >>> # use the get operator to get the value at an index of a list
    >>> (1,"hello",{ "hi" print }) 1 get println
    hello
    ```
# Supported Types/Literals:
* Strings: `"literal enclosed in quotes"` : text
* Numbers: `1 1.0 inf 1e-4`: numerical values
* Null: `null`: a placeholder for a lack of a value
* Reference: `$var`: a reference to another piece of data (or another reference)
* Macro/Block/Stack: `{ }`: a container of code/values which can be run (similar to functions)
* List: `("comma-separated","values",)`: an organized container of values (no value = null)
* Index (Intermediate type): `5 ]`: these are a part of the lazy-evaluation of list indicies.

# An incomplete list of built-in operators, functions, constants, etc.:
* Constants:
  - `true` = 1
  - `false` = 0
  - `null`: passes null object
  - `pi`: pushes approximation of pi
  - `ans`: only available in shell; Pushes value at top of stack

* Mathematical operators:
  - `+`: add (also concatenates when one or more of the arguments is a string)
  - `-`: subtract
  - `*`: multiply
  - `/`: divide
  - `%`: modulus/remainder
  - `**`, `pow`: raise arg1 to arg2 power
  - `sqrt`, `sqr`: square route
  - `log`, `log10`: base 10 logarithm
  - `ln`: natural logarithm
  - `logbase`, `logBase`, `logBASE`: Logarithm of the given base
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

* Stack/flow operators: these are useful in carrying values to their destination in the correct order
  - `...`: empty stack
  - `;`: pop top of stack
  - `dup`: duplicates the value on the top of the stack (1 arg)
  - `dupn`: performs `dup` n number or times (2 args)
  - `swap`: swaps the top 2 elements in the stack
  - `assert`: makes an error if top of stack is false
  - `stklen`: returns the size of the stack
  - `reverse_stack`: reverses the order of stack
  - `range`: puts a range of numbers onto the stack

* Types:
  - `str`: converts to string
  - `num`: converts to a number
  - `int`: converts to a integer (rounding)
  - `list`: puts values on stack into a list
  - `stk`: puts values on stack into a list
  - `floor`: static casts to an int
  - `~`: copy operator, replaces a reference with the basic value it points to

* Variables/References
  - `=`: assignment operator
  - `delete`: deletes a variable/reference
  - `is_defined`: returns whether or not the given variable is defined or not
  - `vars`, `ls_vars`: prints list of variables in all scopes plus debugging info
  - `typeof`: pushes string containing type of a value

* String operators:
  - `strlen`: returns the length of a given string
  - `strstr`: finds a sub-string in a string
  - `stristr`: case-insensitive strstr
  - `trim`: trims whitespace from start and end of the stringsubroutine
  - `split`: splits a string using given delimiters (also splits list)
  - `str_replace`: replaces all occurances of a substring with another substring

* I/O:
  - `print`: prints previous value
  - `println`: prints previous value and ends line (&flushes file)
  - `print_blk`: prints the contents of a macro/block
  - `input`, `getline`: reads a line from terminal (returns a string)
  - `getchar`: reads a character from terminal (returns a string)
  - `color_print`: prints a value in a given HTML color in a string. The color should be come after value
  - `setFgColor`, `setBgColor`: change text color to an html color, passing `null` resets color
      changes the terminal color to a given html color contained in a string
  - `reset_color`, `resetFgColor`, `resetBgColor`: resets background and/or forground colors and effects

* File operators:
  -  `file_get_contents`, `file_put_contents`: convert between files and strings

* Macro Operators:
  - `@`, `eval`: runs the given macro or string as code (also accepts strings)

* List Operators:
  - `get`: pushes the value at the given index of the given list
  - `split`: pushes all the elements of a list onto the stack
  - `n ]`: index for given list, evaluates to that element in the list (behavior similar to reference type)
* Structure-Equivalent Operators:
  - `else`, `elseif`, `if`: conditional operators
  - `repeat`, `while`, `for-each`: looping operators

* Interpreter Commands
  - `q`, `exit`, `quit`: stops interpretation of the script
  - `help`: displays help message
  - `reset`: deletes all variables, stack, etc. (essentially restarts interpreter)
  - `errors-on`, `errors-off`: toggle error messages (on by default)
  - `syscall`: calls `system()` on the given string. (for running system shell commands)


# Related Repos
  When I develop the interpreter I like to develop different portions separately and then add them to my code-base once I've rigorously tested them. If you want to contribute to a particular sub-system, I'd reccomend editing these repos instead of this one.
  - https://github.com/dvtate/stack-demo
  - https://github.com/dvtate/terminal-colors
  - various other programs in https://gist.github.com/dvtate
