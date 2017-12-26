# Engine Schema

The engine is a library. Every application loads the engine, and initializes it.

## `Core`
This is the entry point of any game.

## `Object`
Abstract base class that provides a few very important things (one of them, debatable):
+ A handle to `Sys.Hermes` to receive messages
+ A handle to `Logger` to log stuff
+ A handle to `Sys.Alexandria` to load resources
	- This is the point of (internal) debate... does everyone need to access `Alexandria`? Probably not.
+ An identifier string and its hash.

Every conceivable object will probably have the need to (a) send/receive messages and/or (b) log stuff. This base class provides that. An `Object` is *not safe for use* until `post_init()` has been called, passing handles to systems and providing name information. `post_init()` then calls the pure virtual function `inner_post_init()` specific to each object instance: from that point on, an `Object` is safe for use.

About dependency injection: the general rule is that (a) *whoever creates an `Object` is responible for injecting the necessary handles* and (b) *any object must assume that its handles are valid*. Since injection chain really begins in `Core`, which owns the systems, the issue is practically solved. The `Object` class contains a helper function `post_init_child(Object* o, string name)` that injects handles and name information to the specificed object. This way, Objects can create other Objects and pass references transparently.

## Systems

Most things should *not* be systems. A system is a piece of code that provides an interface to, usually, some external resource. We have a system for asset loading, a system for rendering, a system for audio, etc. In general, anything of which multiple instances are conceivable to exist should *not* be a system. Since it makes no sense to have multiple asset loading class instances, we make it into a system.

Systems are long-lived, so they have to be careful with memory. `Core` creates an instance of each and passes pointers to whoever needs them.

### Sys.Alexandria

Asset manager. Abstracts away machine-specific paths and handles, and provides a uniform interface to access resources.

- `Alexandria::get("/local/maps/foo.lua", type::map)`
- `Alexandria::get("/server/maps/bar.lua", type::map)`

Stuff like this.

Asset discovery can happen in a few different ways:
- `Alexandria::load_database(filename)`: this method opens an Alexandria database, i.e. a `Lua` file containing a description of resources in a table. See `run/alexandria.lua` for an example. Useful for dev environment (flexible).
- `Alexandria::load_binary(filename)`: this should load a tightly-packed binary file. Ideally, we would just `mmap` and copy straight into memory the stuff that needs to be held in RAM, and send to the GPU the stuff that does not. Useful for production environment (fast).

Assets are referenced by strings with format `/Database/Path/Like/This`. Behind the scenes, the string is hashed and the hash looked up on the resource database, i.e. `map<int, asset>`. (String hashes could be computed at compile-time for hard-coded stuff).

#### Ownership & Memory policy

Alexandria for now does not care about memory efficiency in terms of asset loading. The moment you ask for a resource, it is loaded into memory and a simple pointer handle provided. You can ask for an asset to be unloaded and Alexandria will unload it; other parts of code that might be using it are out of luck.

Future: reference counting (`std::shared_ptr`?). Some profiling.

### Sys.Audio

### Sys.Render

The *Sys.Render* system deals with the graphics backend (OpenGL). No other part of the engine should ever talk with OpenGL. This way, we make sure that all the rendering state is contained within `Sys.Render`.

How I currently envision the `Render` system design:
+ Each `GameState` owns a certain amount of `Scene`s, each representing a physical world, a GUI layer, etc. at a *game* level, without caring at all for GPU state. Similarly, it owns a certain amount of `RenderScene`s, each being a specific graphical representation of the scene. The usual case is a single `Scene` representing 3D geometry (in a hirearchical way, easy for programmers to reason about) and a corresponding `RenderScene`, holding a "render"-view of the physical data in the `Scene`. `GameState` owns *a single `EntityManager` and a single `xComponent` manager*, and each scene type peeks into it to provide functionality. The `Scene` cares about lights, models, terrain, cameras, etc. The `RenderScene` only cares about meshes, shader programs, textures, and so on. An attempt at separating concerns and keeping code data-oriented.
+ The `GameState` creates and modifies every `Scene` with a high-level interface: `scene_gui.add_object("car")`, and the `Scene` asks `Alexandria` for the resource, loads it into memory, and keeps track of it as a physical entity and/or a renderable one in its `EntityManager` [q: instance? global?].
+ At every frame, the `GameState` updates the `Scene`s it desires, and calls the `Scene`'s `render()` method.
+ The `Scene` generates `RenderCommand`s for every drawable object (after culling).
+ `Sys.Render` sorts the list of commands (a "job" packet) and renders with as few OpenGL calls as reasonably possible. For this, we need to distill information passed to the rendering system to the minimum.

Having such a compartmentalized workflow paves the ground for parallelization. All of this is really me trying to grok the modern approach of "data-oriented" design. It leads to cleaner division between logical states of a program (update, draw, dispatch) and other nice things, probably. At any rate, I do not want a monster all-encompassing `Game` class. Let each do one thing (or as few as possible) well.

More stuff (15/12/17): the `Render` system running on its own thread is good thing overall, but adds certain complexity to our code. We cannot say `mesh::create()` and expect to get at that exact moment a VBO handle from `Render`, because the render thread might be busy drawing stuff. What happens behind the scenes is that we populate a command buffer, consumed by the render thread at the beginning of each render frame, and only then have the mesh actually uploaded to GPU memory. But that's fine: game-level objects do not really care about the GPU handle, since it's only useful for the render thread itself, and by the time it executes a render command referencing that specific handle, the previous `upload` has already been processed.

We avoid having to "sync with the render thread" by completely delegating the job. The idea is that a scene object can be created, exist, interact with its environment and die *even without having a graphics representation drawn to screen*. The real delay though, will be a single frame at most, which is optimal enough.

(26/12/17): Some words on how the render pipeline is set up. The file `render_config.json` contains most of the stuff required to construct the render system. First, we declare global `pipeline_targets` (render and depth-stencil targets) to be used, with a specific format. Then we declare `pipeline_passes`, that specify either `fullscreen` passes (renders onto a fullscreen quad from given input textures, specifying a shader) or a `layer` (renders onto specified output textures, used by arbitrary shaders).

In our basic setup, we have the following:
1. *`gbuffer` layer pass*, draws 3D geometry onto three targets (`albedo` & `normal` + depth-stencil).
2. *`gui` layer pass*, draws 2D geometry onto two targets (`overlay` + overlay depth-stencil).
2. *`deferred_shading` fullscreen pass*, computes traditional deferred shading from data in `albedo` & `normal` textures, onto the `back_buffer` default target.
3. *`overlay_oass` fullscreen pass*, simple additive [...]

### Sys.Net


### Sys.Script



-----

## Stand-alone classes

### Hermes
The message passer. Lives in the main thread, but is multi-writer-ready. The current implementation is dead simple; a single `vector` of messages that are accumulated and flushed every frame. Writers acquire a lock on-write to push back, and readers do too (we assume there can be a read-write situation, which might be possible when the render thread is spinning on its own). When a game object wants to read messages, it calls `Hermes::pull_inbox(subscription)` which generates on-the-fly a vector of messages directed to one of the cannels the subscription is interested in. This is not ideal. What we can do better:

+ Separate the inbox in a `write_queue` and a `read_queue` (or similar) to allow separate read and write locks (i.e. double buffering).
+ Allocate for each subscription a `ref_queue` and for each message received, push its index into every subscription queue. Pulling the inbox becomes lighter, but with an added overhead at every `send()`.
+ (20/12/17) TODO: change this around. Every writer has a channel. Readers peek through a channel. Fast, no copying.

### GameState

Games are essentially finite state machines. Each of these states is represented by a `GameState` instance. It holds (references) to everything in the scene: spatial information, geometry information, etc.

### Scene

The top-level structure that holds scenes together. A game state might have multiple `Scene`s. The `GameState` plays with the `Scene` to instantiate objects; a spatial component is stored (in the `SceneGraph`), a drawable component, if aplicable, is stored.

Things that the scene manager should implement:
- `add_light`
- `add_camera`
- `add_object` (drawable)

It keeps a list of skeletons, lights, cameras, ... abstracted away in the `SceneGraph`, but made available to the `GameState` that owns it.

(Should the `Scene` interface with `Sys.Alexandria` to load all resources?? A: OF COURSE!)

Two functions are of utmost important:
- `render`: iterates through renderables and generates draw commands.
- `update`: updates every component of the scene based on policies set at creation (physics? etc.)

On the highest level, a `Scene` consists of a vector of `Scene::Node`s (entity). Every node has a set of components, queryable from the corresponding component manager.

### Component.Transform

A simple scene graph implementation; directed tree holding *spatial* information about objects in a scene. The important thing is to notice that *there is no single array of ~things~* in the engine: the `Transform` component provides a *hirearchical spatial representation* of scene physical 'objects' (might be cameras, lights, people, bridges...), while other components might hold *renderable representations* of them. It is the combination of them that is powerful.

This separation of concerns is useful, because it allows us to efficiently traverse a scene spatially, and batch geometry to execute few OpenGL calls by looking at a different, tightly packed array of renderable stuff.

Read [this post by L. Spiro](https://www.gamedev.net/forums/topic/672161-need-scene-graph-advice-please/?tab=comments#comment-5255071) on the function of scene graphs for clarification: *"Scene graphs do not supply anything. They propagate data and transforms down. These transforms may be used for rendering, but that is not the scene graph’s problem. They can also be used for physics or whatever. After the scene graph facilitates the creation of the world coordinates of an object, it sits back and lets the other systems use that data as they please. That’s their problem, not Mr. Graph’s."*

And [more stuff](http://lspiroengine.com/?p=566).

### Material

We need a high-level material editor, graph-based, all that jazz. I envision two types of low-level shaders: a 3D-ready shader, dealing with vertex position, normal, texture coordinates and/or base color, and a 2D-ready shader, dealing with GUI rendering. The right tools for flexibility (no texture? no normals? etc) are [*GLSL subroutines*](https://www.khronos.org/opengl/wiki/Shader_Subroutine). Our material editor must feed the übershader with vertex/color data, through visual editing.