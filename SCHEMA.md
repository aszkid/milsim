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

### Sys.Scripte

The most powerful system of the engine. It has the ability to communicate with every other system, and relay information to script *capsules*. Furthermore, script capsules can communicate with each other. Heavy stuff.


-----

## Stand-alone

### GameState

Games are essentially finite state machines. Each of these states is represented by a `GameState` instance. It holds (references) to everything in the scene: spatial information, geometry information, etc.

### SceneGraph

A simple scene graph implementation; directed tree holding *spatial* information about objects in a scene. Every `GameState` holds some of them. The important thing is to notice that *there is no single array of ~things~* in the engine: the `SceneGraph` provides a *hirearchical spatial representation* of scene 'objects' (might be cameras, lights, people, bridges...), while other structures hold *renderable representations* of them. It is the combination of them that is powerful.

This separation of concerns is useful, because it allows us to efficiently traverse a scene spatially, and batch geometry to execute few OpenGL calls.