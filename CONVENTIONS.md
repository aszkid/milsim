# Coding conventions

- *PascalCase* namespace and class names.
- *under_case* method and function names.
- *x_under_case* variable names.

- `TAB` instead of spaces (set your editor to 4-space-wide tabs).
- Most things from [Linus' coding style](https://www.kernel.org/doc/html/v4.10/process/coding-style.html) with regards to braces, spaces, etc.


## Prefixes

As a codebase grows larger, keeping track of types and ownership is hard. We use a small amount of prefixes to paliate that:

- `m` for members

(from https://stackoverflow.com/questions/1228161/why-use-prefixes-on-member-variables-in-c-classes)


## Exceptions

I use [this advice](https://gamedev.stackexchange.com/questions/46424/try-catch-or-ifs-for-error-handling-in-c):

> The general line of thinking is pretty clear: use exceptions for exceptional circumstances. Use them when your program gets into a "I have no idea what to do, maybe hopefully someone else does, so I'll throw an exception and see what happens." Use them when no other option makes sense. Use them when you don't care if you accidentally leak a little memory or fail to clean up a resource because you messed up the use of proper smart handles. In all other cases, don't use them.
