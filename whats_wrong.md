
# what's wrong with the old interpreter?
TLDR: Because it was initially designed to be a calculator and not as a scripting language, artifacts and design choices stayed and eventually the scale grew to a point where I could no longer make changes needed to add more features without a complete rewrite.
- it has a function called `process_line` which takes a minimum of 7 arguments, of which most consist of templated reference and pointer types.
- it didn't take memory management seriously enough memory leaks existed
- it used lots and lots of tempfiles for no good reason. 
  + instead of creating a proper buffer to hold code that needed to be executed, I created tempfiles and filled them with the code.
- 2.5k line if statements and other atrocities
- the poor decisions i made early on it it's development forced me to implement things in weird and unintuitive ways later on.
  + furthermore, poor early decisions forced inefficient decisions which prevented efficient solutions later on and so forth
  + premature optimization isn't good
- I made the infastructure to for language extensions, however I dont think anyone in their right mind would be able to learn how the interpreter worked
- linked lists where there was no reason for them
- everything was muteable and overwritten
- negative & massively incorrect line and character locations in error messages (also including C++ files in msg)
  + this is partially associated with the language's design
  + giving feedback comparable to most programming/scripting languages would reduce performance
    * imagine everything that could ever get run has to have a character number and file name associated with it
  + still what I had before was totally inadequate and needs improvement

# whats new for yodascript?
- arbitrary precision integer datatype
- variables are now references to data
  + all references are now to data and not to variable names which change meaning with scope
  + expect to have to use more (`~`) operators
    * honestly probably less tho bc no more issues with name collisions
- operator overloading is coming soon (should be implemented similar to python)
- syntactic sugar for lambda args/params changed
  + instead of variadic args and missing handlers, the plan is to pass an args list containing all the arguments (similar to javascript)
  + this will be less cool but will mean significantly improved performance for lambdas
- object improvements
  + I have a lot of iteas and I think it's a little early to start explaining
  + Perhapse syntax changes, I like the current syntax because it can be written similar to JSON, but it's somewhat vague
    * I really like scala's OOP, might use it for some pointers
  + a real sense of self is coming now that I have proper references
  + might take some more pointers from javasript
- names of operators probably gonna changes (conventions inc)
- might move some features into precomiled modules (shouldn't hurt performance asside from cache misses)
- could eventually add multithreading (interpreter design won't be completely opposed to it)
- goto's and exception handling are now possible with new interpreter design.
- 

# whats new for the interpreter?
- no more lazy evaluation
  + variables/storage locations will be created as soon as they are named, but will be set to `empty` (aka undefined)
  + this approach might run into issues with lists, will see
- proper input buffering that's compatible with both files and shell
- everything associated with given scope contained in a `Frame` object
- no more global systems which can't be forked (ie - variables now handled by individual `Frames` instead of globlaly)
- more STL and less DIY, I took very C-style approaches to things in the old interpreter, imma try C++ style this time
- 
