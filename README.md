# RPN Calculator
A simple calculator that uses reverse polish notation. 

# How to use
 - <b>Comments:</b>
   Anything after the `#` symbol will be ignored.
   ```
   > 32 # this is a comment
   ans = 32
   ```
   
 - <b>Binary operators:</b>
  Place the operator after it's two operands.
   ```
   > 1 2 +    # 1 + 2 
   ans = 3
   > 2 5 **   # 2 ^ (5)
   ans = 3125
   ```
   
 - <b>Unary operators/funcitons:</b>
  place the unary operator/funciton behind it's parameter
  ```
  > 100 log   # log10(100)
  ans = 2
  > 90 sine   # sine of 90 radians
  ans = 0.893997
  ```
  
 - <b>Combining operators:</b>
  One of RPN's main strengths is that it eliminates the need for parenthases. Simply combine operators in a way that would lead to them being performaed sequentially.
  ```
  > 1 1 + 2 * # (1 + 1) * 2  
  ans = 4
  ```
  
 - <b>Keywords:</b>
    * `> help` <br/>
      The help command displays a short dialog to help get you started.
    *`> clear` <br/>
      clears the terminal
    * `> reset` <br/>
      resets the line numbering
    * `> exit` <br/>
      closes the program
    * `> q` <br/>
      same as `exit`
    * `> ans` <br/>
      previous answer, initialized to 0.
    * `> pi` <br/>
      an approximate value for pi
