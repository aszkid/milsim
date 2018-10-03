# milsim

A game engine in (you've guessed it) C++. These too-many 6,000 LOC provide the (WIP) design of a somewhat backend-agnostic rendering pipeline (take a look at the [render.hpp](engine/include/sys/render.hpp) system), basic resource loading (`.obj` model files, textures in most reasonable image formats, fonts, shaders) and a message-passing system ([hermes.hpp](engine/include/hermes.hpp)).

The 'logic' and 'render' steps happen concurrently, but work on different frames. While the logic thread is working on submitting draw calls an updating world state for frame `N`, the render thread is processing batched draw calls and resource commands from frame `N-1`. This avoids hairy concurrency issues insofar as logic-render interactions are concerned, and defines a very clear line separating concerns between engine systems. The built-in render backend implementation is OpenGL, because that's what I'm familiar with.

*Project architecture:* the engine compiles into a library, while apps link from it and design their own game states. We use CMake for cross-platform building; the engine itself should work on Linux and Windows at least (tested on Linux x64).

*Design philosophy:* a somewhat rambling write-up on the different parts of the engine can be found in the [SCHEMA](SCHEMA.md) document. The [bitsquid](http://bitsquid.blogspot.com/) (later 'Stingray') engine was a major source of inspiration with regards to data-oriented patterns. Take a look at [ROADMAP](ROADMAP.md) for a progress check-list.

## Demo screenshot

Rendering an OBJ model textured by a MTL file. Interactive 3D camera (FPS-like).

![Example of basic 3D rendering](test.jpg)

## Startup

1. A core engine object is created.
2. The message passing system is intialized.
3. All systems are initialized (input, render--from [render_config.json](run/render_config.json), resource loading).
4. The resource loading system collects a database of resources from a JSON file (e.g. [Base.json](run/Base.json)).
5. An initial game state is added to the list of states (which might look something like [main_screen.cpp](app/states/main_screen.cpp)).
6. The core object starts [looping](engine/src/core.cpp#L98); message queues are swapped at every frame.
