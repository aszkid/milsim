# milsim
A big, lonely project

![Example of basic 3D rendering](test.jpg)

## Headstart
 - The `engine/sys/render.cpp` system, a separate thread that constructs a render pipeline from `render_config.json` and receives "abstract" drawing commands.
 - The `engine/sys/alexandria.cpp` system, the resource loader.
 - `app/main.cpp`, an example entry point.
