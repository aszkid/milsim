# Engine Schema

The engine is a library. Every application loads the engine, and initializes it.

## Core
This is the entry point of any game. While simple by itself, its function is to load every engine system and hook them up for effective communication.

The communication system is a subscription-based message queue. Every system is given a set of subscription parameters (priorities, frequency, etc) and the communication system balances the load of messages.

## Systems

### Sys.Hermes

Messaging system. Lowest level of communication between engine systems.

### Sys.Alexandria

Asset manager. Abstracts away machine-specific paths and handles, and provides a uniform interface to access resources.

- `Alexandria::get("/local/maps/foo.lua", type::map)`
- `Alexandria::get("/server/maps/bar.lua", type::map)`

Stuff like this.

### Sys.Audio

### Sys.Graph

The *Sys.Graph* system (graphics) deals with both 3D and 2D stuff in a relatively transparent way. It is capable of generating UI layouts (howw???) and drawing 3D scenes.

A `GameState`

### Sys.Graph.GUI

A subsystem that specializes on drawing GUIs. A GUI screen is a tree of *widgets*, anchored to a parent frame. Every single piece of GUI is loaded from scripts.

### Sys.Net

### Sys.Script

The most powerful system of the engine. It has the ability to communicate with every other system, and relay information to script *capsules*. Furthermore, script capsules can communicate with each other. Heavy stuff.


-----

## Stand-alone

### GameState

Games are essentially finite state machines. Each of these states is represented by a `GameState` instance. It holds (references) to everything in the scene: spatial information, geometry information, etc.

### SceneManager

The top-level structure that holds scenes together. A game state might have multiple `SceneManager`s. The `GameState` plays with the `SceneManager` to instantiate objects; a spatial component is stored (in the `SceneGraph`), a drawable component, if aplicable, is stored.

Things that the scene manager should implement:
- `add_light`
- `add_camera`
- `add_object` (drawable)

It keeps a list of skeletons, lights, cameras, ... abstracted away in the `SceneGraph`, but made available to the `GameState` that owns it.

(Should the `SceneManager` interface with `Sys.Alexandria` to load all resources??)

Two functions are of utmost important:
- `render`: does all necessary culling, batching, etc. based on the `SceneGraph` and `SceneDrawable` objects.
- `update`: updates every component of the scene based on policies set at creation (physics? etc.)

### SceneGraph

A simple scene graph implementation; directed tree holding *spatial* information about objects in a scene. Every `SceneManager` holds some of them. The important thing is to notice that *there is no single array of ~things~* in the engine: the `SceneGraph` provides a *hirearchical spatial representation* of scene 'objects' (might be cameras, lights, people, bridges...), while other structures hold *renderable representations* of them. It is the combination of them that is powerful.

This separation of concerns is useful, because it allows us to efficiently traverse a scene spatially, and batch geometry to execute few OpenGL calls.

Read [this post by L. Spiro](https://www.gamedev.net/forums/topic/672161-need-scene-graph-advice-please/?tab=comments#comment-5255071) on the function of scene graphs for clarification: *"Scene graphs do not supply anything. They propagate data and transforms down. These transforms may be used for rendering, but that is not the scene graph’s problem. They can also be used for physics or whatever. After the scene graph facilitates the creation of the world coordinates of an object, it sits back and lets the other systems use that data as they please. That’s their problem, not Mr. Graph’s."*

And [more stuff](http://lspiroengine.com/?p=566).