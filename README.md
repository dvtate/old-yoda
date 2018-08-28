# YodaScript / RPN Shell
A YodaScript (.ys) interpreter which is now turing complete! The language is dynamically typed, and although it has a rather unique syntax, those familiar to RPN may prefer it to other languages. This is just a side project of mine suggestions and help are welcome. Using the shell, it can be used as a calculator by those familiar with RPN.

# Build and Run [![Build Status](https://travis-ci.org/dvtate/yoda.svg?branch=master)](https://travis-ci.org/dvtate/yoda/)
For Arch Linux users there is an [AUR package](https://aur.archlinux.org/packages/ys-git/) for this.
If you are a Windows user and are slightly confused, you can download this outdated executable
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

# Features
For an up to date features list click [here](https://plus.google.com/+Tatetesta/posts/XqoHQkATify).
    
# Types/Literals:
* Strings: `"literal enclosed in quotes"`: text
* Numbers: `1 1.0 inf 1e-4`: numerical values, or `true`=1/`false`=0
* Null: `null`: a placeholder for a lack of a value
* Reference: `$var`: a reference to another piece of data (or another reference)
* Macro/Block/Stack: `{ }`: a container of code/values which can be run (similar to functions)
* List: `("comma-separated","values",)`: an organized container of values (no value = null)
* Index (Intermediate type): `5 ]`: these are a part of the lazy-evaluation of list indicies.
* Lambdas: `{ } ($param1,$param2) lambda`: lambdas are first class functions and can be treated as normal data
* UserType: `{} object`: agregated data (lambdas are data) more commonly known as objects
* Member Request (Intermediate type): `:getName`: these are used to access object members


# How to use (note- may be out of date)
 - <b>Comments:</b>
   Anything after the `#` symbol will be ignored.
   ```Tcl
   >>> 32 # this is a comment
   32
   ```

 - <b>Binary operators:</b>
  Place the operator after it's two operands.
   ```Tcl
   >>> 1 2 +    # 1 + 2
   3
   >>> 2 5 **   # 2 ^ (5)
   32
   ```

 - <b>Unary operators:</b>
  place the unary operator/funciton behind it's parameter
  ```tcl
  >>> 100 log   # log10(100)
  2
  >>> 0.5 sin   # sine of 0.5 radians
  0.479426
  ```

 - <b>Combining operators:</b>
  One of postfix notation's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performed sequentially.
  ```tcl
  >>> 1 1 + 2 *    # (1 + 1) * 2  
  4
  ```
 - <b>Variables:</b>
  Variables are created as soon as you start using them. All variable names must start with a `$` to prevent them from clashing with other keywords and literals. Here's an example:
```tcl
 >>> $a 4 =
 >>> $a print
 4
```
  Variables are references to data, variables can reference eachother. Changing the value of a reference changes the value  of it's referenced variable.  If you want to make a deep copy, use the copy operator (`~`). Also, note: this language uses dynamic scope resolution and variables are deleted as they go out of scope. If 2 variables in different scopes have the same name, the one in the more specific scope will be used.
```tcl
>>> $a 8 =    # now $a is a reference to the number 8
>>> $b $a =   # now $b is a reference to $a and $a is a reference to the number 8
>>> $b $a ~ = # now they are both references to the number 8
```
 - <b>Strings:</b>
  This is a loosely-typed language. Strings are enclosed in double quotes `"`
 ```tcl
 >>> "hello " "there" + $a = # notice that the `+` operator is overloaded
 >>> $a print
 hello there
 ```
 - <b>Macros:</b>
  Depending on the context these are also called anonymous subroutines. They are containers of code and function like most other data, working with operators. You can run the code in one with the `@` operator.
  ```tcl
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
   ```tcl
   >>> { "ha" print } 5 repeat
   hahahahaha
   ```
  - <b>While Loops:</b>
   runs core while a condition is met (checks before each cycle)
   ```tcl
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
   ```tcl
   1>>> {
	     $num print  
   } (0, 1, 2, 3, 4) $num foreach
   01234
   ```
 - <b>Conditionals:</b>
  These can be used to run code if a condition is met
  ```tcl
  >>> "Enter GPA: " print $gpa input int =
  Enter GPA: 4
  >>> #this first method is syntactic sugar and slower than the second
  >>> { "You can do better than that." println 
  } else { 
      "That's good enough for me." println 
  } $gpa 3 >= elseif { 
      "Woooooooooooow!" println 
  } $gpa 4 >= if
  Woooooooooooow!
  ```
 - <b>Lists:</b>
  lists hold multiple pieces of data in one container. They contain diverse types and are handled like any other data.
  ```tcl
  >>> # use the get operator to get the value at an index of a list
  >>> (1,"hello",{ "hi" print }) 1 get println
  hello
  ```
 - <b>Lambdas:</b>
  Lambdas are equivalent to arrow-functions in javascript.
  ```tcl
  >>> $fxn { $a print } ($a) lambda =
  >>> ("hello") $fxn @ # calling the lambda
  hello
  >>> # parameters are optional
  >>> # return pushes the last value and ends the lambda
  >>> { 5 return } () lambda @ print
  5
  >>> # break stops the lambda and carries the entire stack
  >>> { 4 5 break } () lambda @ + print
  9 
```
  - <b>Missing handlers:</b>
   These will run a macro if the parameter isn't provided as an argument. 
   ```tcl
   >>> $echo {
        { $msg println } 
            $msg is_defined if
   } (($msg, {
       "missing $msg" println 
   })) lambda =

   >>> ("hi") $echo @
   hi
   >>> $echo @
   missing $msg
   >>> () $echo @
   missing $msg
```
  - <b>Variable Arguments list:</b>
   If you have a function which needs to accept a variable number of arguments (ie - [printf](examples/printf.ys)), you can end your parameters list with a variable enclosed in a list, denoting a variable arguments list. If not called on anything, the variable is assigned to an empty list. 
   ```tcl
   >>> $sum {
        $total 0 = 
        { $total $n += } 
            $vals $n foreach
        $total return
    } (($vals)) lambda =
    >>> (1,2,3) $sum @
    6
    >>> $sum @
    0
```

 - OOP:
  - Making an Object:
   You can use the equals sign to define objects. The variable doesn't have to be defined in order to start adding members. The `:` operator is used to access object members. The object will be made around the member.
   ```tcl
   >>> $myObj :name "bob" =
   >>> $myObj :name print
   bob
   >>> # {} object makes an empty object constant
   >>> $myObj {} object =
   ```
   


# An incomplete list of built-in operators, functions, constants, etc.:
* Constants:
  - `true` = 1
  - `false` = 0
  - `null`: passes null object
  - `pi`: pushes approximation of pi
  - `ans`: only available in shell; Pushes value at top of stack

* Mathematical operators:
  - `<v1> <v2> +`: add (also concatenates when one or more of the arguments is a string)
  - `<minued> <subtrahend> -`: subtract
  - `<v1> <v2> *`: multiply
  - `<numerator> <denominator> /`: divide
  - `<numerator> <denominator> %`: modulus/remainder
  - `<base> <exp> **`, `pow`: raise arg1 to arg2 power
  - `<radicand> sqrt`, `sqr`: square route
  - `<logand> log`, `log10`: base 10 logarithm
  - `<logand> ln`: natural logarithm
  - `<logand> <base> logbase`: Logarithm of the given base
  - `<angle> sin`, `cos`, `tan`: trigonometric funcitons (radians)
  - `<value> asin`, `acos`, `atan`: inverse trigonometric funcitons (radians)
  - `<angle> sinh`, `cosh`, `tanh`: hyperbolic trigonometric functions (radians)
  - `<value> asinh`, `acosh`, `atanh`: inverse hyperbolic trigonometric functions (radians)

* Logical operators: (0 is false everything else is true)
  - `<cond1> <cond2> &&`: logical-and (use macro enclosed boolean expressions for short-circuit logic)
  - `<cond1> <cond2> ||`: logical-or (use macro enclosed boolean expressions for short-circuit logic)
  - `<cond> !`: logical-not
  - `<cond1> <cond2> !=`: logical-XOR

* Compairison operators:
  - `<v1> <v2> ==`, `?=`: equals-to
  - `<v1> <v2> !=`: not equals-to
  - `<v1> <v2> >`: greater than
  - `<v1> <v2> <`: less than
  - `<v1> <v2> >=`: greater than or equal to
  - `<v1> <v2> <=`: less than or equal to

* Bitwise operators:
  - `<v1> <v2> |`: bitwise-or
  - `<v1> <v2> &`: bitwise-and
  - `<v1> <v2> ^`: bitwise-XOR
  - `<integer> <offset> <<`: bitshift left
  - `<integer> <offset> >>`: bitshift right

* Stack/flow operators: these are useful in carrying values to their destination in the correct order
  - `...`: clear the stack
  - `;`: pop top of stack
  - `<value> dup`: duplicates the value on the top of the stack (1 arg)
  - `<value> <n> dupn`: performs `dup` n number or times (2 args)
  - `<v1> <v2> swap`: swaps the top 2 elements in the stack
  - `<boolean> assert`: makes an error if top of stack is false
  - `stklen`: returns the size of the stack
  - `reverse_stack`: reverses the order of stack
  - `<start> <end> range`: returns list of integers on interval \[start, end)

* Types:
  - `<value> str`: converts to string
  - `<value> num`: converts to a number
  - `<value> int`: converts to a integer (rounding)
  - `<value> floor`: static casts to an int (truncation)
  - `<value> list`: puts values on stack into a list
  - `<body macro> <parameters list> lambda`, `lam`: make a lambda (creation not conversion)
  - `<members macro> object`, `obj`: make an object (creation not conversion)
  - `<reference> ~`: copy operator, replaces a reference with the value it points to

* Variables/References
  - `<reference> <value> =`: assignment operator (shallow copy by default, for deep combine with `~`)
  - `<reference> delete`: deletes a variable/reference
  - `<reference> is_defined`: returns whether or not the given variable is defined or not
  - `vars`: prints list of variables in all scopes plus debugging info
  - `<reference> typeof`: pushes string containing type of a value
  - `<reference> <modifier> +=`, `-=`, `*=`, `/=`, `<<=`, `>>=`: modified assignment operators
  - `<reference> ++`, `--`: increment/decrement operator
  - `<reference> refstr`: returns a string containing the reference label
  - `<label string> $`: makes a reference from a string containing the label (opposite of `refstr`)
  - `<value> global_ref`: allocates a global reference with a mutilated name (call `delete` later)
  
* String operators:
  - `<string> strlen`: returns the length of a given string
  - `<haystack string> <needle string> strstr`: finds a sub-string in a string (returns `null` otherwise)
  - `<string> <substring> stristr`: case-insensitive strstr
  - `<string> trim`: trims whitespace from start and end of the stringsubroutine
  - `<string> <delims string> split`: splits a string using given delimiters (also splits list)
  - `<string> <substr1> <substr2> str_replace`: replaces all occurances of a substring with another substring

* I/O:
  - `<value> print`: prints previous value
  - `<value> println`: prints previous value and ends line (&fflushes stdout)
  - `input`, `getline`: reads a line from terminal (returns a string)
  - `getchar`: reads a character from terminal (returns a string)
  - `color_print`: prints a value in a given HTML color in a string. The color should be come after value
  - `<color string> setFgColor`, `setBgColor`: change text color to an html color, passing `null` resets color
      changes the terminal color to a given html color contained in a string
  - `reset_color`, `resetFgColor`, `resetBgColor`: resets background and/or forground colors and effects

* File operators:
  - `<file string> file_get_contents`: read a file as a string
  - `<contents string> <file string> file_put_contents`: write a string as a file

* Lambda and Macro Operators:
  - `<macro/lambda/string> @`, `<macro/lambda/string> eval`: runs/calls the given macro, lambda or string as code
  - `return`: pushes the top value to the stack and exits the call
  - `break`: keeps the stack and exits the call
  
* List Operators:
  - `<list> <index number> get`: pushes the value at the given index of the given list
  - `<list> split`: pushes all the elements of a list onto the stack
  - `<list> <index number> ]`: index for given list, lazy evaluates to that element in the list
  - `<list> <value> push_back`: pushes a value onto the end of a list, or creates a list containing the original and added value
  
* OOP:
  - `<object> :member`, `<object> <member string> :`: member accessors
  - `<members macro> object`, `obj`: object constuction/definition
  
* Structure-Equivalent Operators:
  - `<macro> else`, `<macro> <condition> elseif`, `<macro> <condition> if`: conditional operators
  - `<conditions macro> cond`, `condition`: an if elseif...elseif else statement similar to a switch
  - `<body macro> <times number> repeat`: run `body` `times` times
  - `<body macro> <condition macro> while`: run `body` while `condition` is true
  - `<body macro> <elements list/macro> <iterator reference> foreach`: run `body` for each element of `elements` stored in `iterator`

* Timing:
  - `<miliseconds number> sleep`: pauses interpretation for the given number of miliseconds (microsecond accuracy*)
  
* Libraries and Extending the Language:
  - `<macro> <label string> define`: the interpreter will run the `macro` for each `label` token seen. (tokens don't have spaces)
  - `<filename string> insert`: runs `filename` (path to yodascript file) in current scope
  - `<library string> load_lib`: adds defs from `library` (path to .so/.dll language extension)
  
  
* Interpreter Commands:
  - `q`, `exit`, `quit`: stops interpretation of the script
  - `help`: displays help message
  - `reset`: deletes all variables, stack, etc. (essentially restarts interpreter)
  - `__errors-on`, `__errors-off`: toggle error messages (on by default)
  - `__errors`: returns 1 if errors are on and 0 if off
  - `<command string> syscall`: calls `system()` on the given string. (for running system shell commands)
  - `__file`: returns the name of the main file being run
  - `__version`: returns version string

# Related Repos & Gists
  When I develop the interpreter I like to develop different portions separately and then add them to my code-base once I've rigorously tested them. If you want to contribute to a particular sub-system, I'd reccomend editing these repos instead of this one. Eventually these will primarily be external libararies
  - https://github.com/dvtate/stack-demo
  - https://github.com/dvtate/terminal-colors
  - various other test-programs in https://gist.github.com/dvtate
