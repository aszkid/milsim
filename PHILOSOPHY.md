# Philosophy of MilSim

This project has a clear goal: to construct a flexible and cross-platform 3D engine focused on military simulator games. Since we can't have all the nice things in the world, we place our attention on two (?) important ones:

1. Functionality over Looks
2. Extensibility over Monolithism

## (1) Functionality over Looks

Our main goal is to provide a complete set of tools that allow developers to do most things in a reasonable way: infantry combat, vehicles, stories, missions, etc. While we acknowledge the important of graphics, the priority of the things one can do with the project vs. how good they look is clear.

## (2) Extensibility over Monolithism

We plan to design the components of the system in a profoundly modular way. Moreso, the core engine should provide only with [sic.] core functionality, allowing game developers to avoid touching C++ code directly for the most part.


----

In terms of code: MilSim is C++-based at its core. We use modern C++11 features without fear, but we try to keep a sensible design, without over-complicating things (e.g. templates are sparingly used).
