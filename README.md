# RPN Calculator
A simple calculator that uses reverse polish notation. 

# How to use
 - Comments:
   Anything after the `#` symbol will be ignored.
   ```
   > 32 # this is a comment
   ans = 32
   ```
   
 - Binary operators:
  Place the operator after it's two operands.
   ```
   > 1 2 +    # 1 + 2 
   ans = 3
   > 2 5 **   # 2 ^ (5)
   ans = 3125
   ```
   
 - Unary operators/funcitons:
  place the unary operator/funciton behind it's parameter
  ```
  > 100 log   # log10(100)
  ans = 2
  > 90 sine   # sine of 90 radians
  ans = 0.893997
  ```
  
 - Combining operators:
  One of RPN's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performaed sequentially.
  ```
  > 1 1 + 1 -
  ans = 1
  ```
  
 - Keywords
    > help
      The help command displays a short dialog to help get you started.
    > clear
      clears the terminal
    > reset
      resets the line numbering
    > exit
      closes the program
    > q
      same as `exit`
    > ans
      previous answer, initialized to 0.
    > pi
      an approximate value for pi
   
