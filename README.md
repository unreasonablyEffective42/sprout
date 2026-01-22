sprout is a purely functional, typed, lazy by default lisp based on system F with full recursion, TCO, and delimited continuations. Planned features include pattern matching, partial application, and both raw one-shot delimited continuations via shift/reset, but also user facing event handlers based on delimited continuations for generators, coroutines and cooperative multitasking. 

The current build is simple, you just need a gcc version supporting C++ 23 as well as MAKE. 

To build just clone the repository and from the directory run make. 

Milestones 

runtime types   done  <br>
lexer           done  <br>
parser          done  <br>
typechecker     wip   <br>
lowering to IR   <br>
Stack vm         <br>
codegen          <br>
