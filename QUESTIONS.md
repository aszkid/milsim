# Questions

Various questions and issues.

+ The `Input` system favours indirect handling: every interested component simply has to subscribe to some input-like event to get input messages. This is fine if everything works perfectly well and no messages are dropped. If an `Object` interested in input is created *after*, say, we have started pressing a key, there is no way for it to learn about that, until we release and press the key again. Plus, an object might receive the message, but not want to act upon receival, rather wait some amount of frames/time. There are more situations like these that favour a *polling-like* system for input handling, i.e. `m_input->get_key(key::W)` instead of checking through messages. Think about this...

+ *Q*: The `Graphics` system... do we really need it? Rendering logic seems to be well-encapsulated in the `Scene` class.
+ *A*: Yes, we need it! Let `Scene` take care of handling scene organization, and leave `Sys.Render` with the heavy lifting in terms of rendering.

+ *Q*: Should `EntityManager` be a system? What about each `xComponentManager`?

+ *BigQ:* We should seriously consider abstracting away render resources. `Sys.Render` should *not* be calling `Alexandria`: we take care of that in each `Scene`! So we need a nice way to bring render resources to a lower-level, more render-friendly representation (after all, we only load a mesh once, but render it thousands of times).
+ *A*: How about having a `RenderResource` light-weight class from which `ShaderProgramAsset`, `ModelAsset`, etc. all derive? We keep a "handle" in the base class (populated by each derived) and so `Sys.Render` only has to upcast, if at all, check the resource type and render away. This is [how *Stingray* works](http://bitsquid.blogspot.com/2017/02/stingray-renderer-walkthrough-2.html) (kind of), and it seems very appropiate. We end up having *two representations* of the "same" data, although this correspondence is not strictly bijective: some game-level entities might be bound to the same render-level resource, and some might not even have a render-level representation.