# Engine Schema

The engine is a library. Every application loads the engine, and initializes it.

## Core
This is the entry point of any game. While simple by itself, its function is to load every engine system and hook them up for effective communication.

The communication system is a subscription-based message queue. Every system is given a set of subscription parameters (priorities, frequency, etc) and the communication system balances the load of messages.

# Sys.Comm

# Sys.Audio

# Sys.Graph

The *Sys.Graph* system (graphics) deals with both 3D and 2D stuff in a relatively transparent way. It is capable of generating UI layouts (howw??? embed chromium? ) and drawing 3D scenes.

## Sys.Graph.GUI

A subsystem that specializes on drawing GUIs. A GUI screen is a tree of *widgets*, anchored to a parent frame. Every single piece of GUI is loaded from scripts.

# Sys.Net

# Sys.Script

The most powerful system of the engine. It has the ability to communicate with every other system, and relay information to script *capsules*. Furthermore, script capsules can communicate with each other. Heavy stuff.
