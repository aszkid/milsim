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
- `Alexandria::load_database(filename)`: this method opens an Alexandria database, i.e. a `Lua` file containing a description of resources in a table. See `run/alexandria.lua` for an example.

Assets are referenced by strings with format `Database.Type.Name`. Behind the scenes, the string is hashed and the hash looked up on the resource database, i.e. `map<int, asset>`. (String hashes could be computed at compile-time for hard-coded stuff)

#### Ownership & Memory policy

Alexandria for now does not care about memory efficiency in terms of asset loading. The moment you ask for a resource, it is loaded into memory and a simple pointer handle provided. You can ask for an asset to be unloaded and Alexandria will unload it; other parts of code that might be using it are out of luck.

Future: reference counting (`std::shared_ptr`?). Some profiling.

### Sys.Audio

### Sys.Render

The *Sys.Render* system deals with the graphics backend (OpenGL). No other part of the engine should ever talk with OpenGL, except for the `RenderScene` class (dedicated to *uploading* data to the GPU). This way, we make sure that all the rendering state is contained within `Sys.Render`.

How I currently envision the `Render` system design:
+ Each `GameState` owns a certain amount of `Scene`s, each representing a physical world, a GUI layer, etc. at a *game* level, without caring at all for GPU state. Similarly, it owns a certain amount of `RenderScene`s, each being a specific graphical representation of the scene. The usual case is a single `Scene` representing 3D geometry (in a hirearchical way, easy for programmers to reason about) and a corresponding `RenderScene`, holding a "render"-view of the physical data in the `Scene`. `GameState` owns *a single `EntityManager` and a single `xComponent` manager*, and each scene type peeks into it to provide functionality. The `Scene` cares about lights, models, terrain, cameras, etc. The `RenderScene` only cares about meshes, shader programs, textures, and so on. An attempt at separating concerns and keeping code data-oriented.
+ The `GameState` creates and modifies every `Scene` with a high-level interface: `scene_gui.add_object("car")`, and the `Scene` asks `Alexandria` for the resource, loads it into memory, and keeps track of it as a physical entity and/or a renderable one in its `EntityManager` [q: instance? global?].
+ At every frame, the `GameState` updates the `Scene`s it desires, and calls the `Scene`'s `render()` method.
+ The `Scene` generates `RenderCommand`s for every drawable object (after culling).
+ `Sys.Render` sorts the list of commands (a "job" packet) and renders with as few OpenGL calls as reasonably possible. For this, we need to distill information passed to the rendering system to the minimum.

Having such a compartmentalized workflow paves the ground for parallelization. All of this is really me trying to grok the modern approach of "data-oriented" design. It leads to cleaner division between logical states of a program (update, draw, dispatch) and other nice things, probably. At any rate, I do not want a monster all-encompassing `Game` class. Let each do one thing (or as few as possible) well.

### Sys.Net

Deals with networking.

### Sys.Script

It has the ability to communicate with every other system, and relay information to script *capsules*. Furthermore, script capsules can communicate with each other. Heavy stuff.


-----

## Stand-alone classes

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
- `render`: does all necessary culling, batching, etc. based on the `SceneGraph` and `SceneDrawable` objects.
- `update`: updates every component of the scene based on policies set at creation (physics? etc.)

On the highest level, a `Scene` object contains a vector of `Scene::Node`s, very much a la Godot. Internally, a `Node` may reference a `Drawable` component, a `Physics` component, etc. by having children.

On the highest level, a `Scene` consists of a vector of `Scene::Node`s (entity). Every node has a set of components, queryable from the corresponding component manager.

### SceneGraph

A simple scene graph implementation; directed tree holding *spatial* information about objects in a scene. Every `Scene` holds some of them. The important thing is to notice that *there is no single array of ~things~* in the engine: the `SceneGraph` provides a *hirearchical spatial representation* of scene 'objects' (might be cameras, lights, people, bridges...), while other structures hold *renderable representations* of them. It is the combination of them that is powerful.

This separation of concerns is useful, because it allows us to efficiently traverse a scene spatially, and batch geometry to execute few OpenGL calls.

Read [this post by L. Spiro](https://www.gamedev.net/forums/topic/672161-need-scene-graph-advice-please/?tab=comments#comment-5255071) on the function of scene graphs for clarification: *"Scene graphs do not supply anything. They propagate data and transforms down. These transforms may be used for rendering, but that is not the scene graph’s problem. They can also be used for physics or whatever. After the scene graph facilitates the creation of the world coordinates of an object, it sits back and lets the other systems use that data as they please. That’s their problem, not Mr. Graph’s."*

And [more stuff](http://lspiroengine.com/?p=566).