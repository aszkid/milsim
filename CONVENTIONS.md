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
