# RPN Calculator
A simple calculator that uses reverse polish notation. 

# How to use
 - <b>Comments:</b>
   Anything after the `#` symbol will be ignored.
   ```
   > 32 # this is a comment
   ans 32 =
   ```
   
 - <b>Binary operators:</b>
  Place the operator after it's two operands.
   ```
   > 1 2 +    # 1 + 2 
   ans 3 =
   > 2 5 **   # 2 ^ (5)
   ans 32 =
   ```
   
 - <b>Unary operators/funcitons:</b>
  place the unary operator/funciton behind it's parameter
  ```
  > 100 log   # log10(100)
  ans 2 =
  > 90 sin   # sine of 90 radians
  ans 0.893997 =
  ```
  
 - <b>Combining operators:</b>
  One of RPN's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performed sequentially.
  ```
  > 1 1 + 2 * # (1 + 1) * 2  
  ans 4 =
  ```
 - <b>Variables:</b>
  This calculator supports user-defined variables created as soon as you start using them. All variable names must start with a `$` to prevent them from clashing with other keywords, functions and numbers. Here's how to define one:
 ```
 > $a 4 =
 ans 4 =
 ```
  You can access the variable by using it's name. You can treat variables just as a regular numerical constant, the only difference is you can change it's value. You can even use it to set a variable to another variable's value.
 ```
 > $b $a =
 ans 4 =
 ```


 - <b>Keywords and Commands:</b>
    * `> help` <br/>
      The help command displays a short dialog to help get you started.
    *`> clear` <br/>
      clears the terminal
    * `> reset` <br/>
      resets the line numbering and variables
    * `> exit` <br/>
      closes the program
    * `> q` <br/>
      same as `exit`
    * `> ans` <br/>
      previous answer, initialized to 0. (access it as you would a variable)
    * `> pi` <br/>
      an approximate value for pi (access it as you would `ans`)


# Build and Run
If you are a windows user and don't know anything about C++ compilers, you can download this precompiled executable (old). <b>Warning</b>, I cross-compiled this on Linux and haven't tested it on a genuine windows box yet, so there could be errors.<br/>
https://onedrive.live.com/embed?cid=8C35060E130BADA5&resid=8C35060E130BADA5%2186540&authkey=AFZY5jRHzaQ1Ltc
- Compiling:
`rpn $ g++ *.cpp -o rpn`
- Running:
`rpn $ ./rpn`
