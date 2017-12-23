# Roadmap

## Immediate Priorities
- [x] Skeleton `Alexandria` implementation.
- [x] Base `Object` class that has a handle to systems.
	+ [ ] Rethink whether `Object` needs a handle to `Alexandria`...
- [x] Macro for `Objects` to initialize sub-`Objects` in a nice way (i.e. don't have to manually pass `m_hermes`, etc).

- [x] Control camera with mouse
- [ ] "Simple" rendering pipeline implementation.
	+ [x] `.obj` loader for Alexandria
	+ [x] Support for textures!
	+ [x] Thread-safe `queue` implementation for Main-Render threads communication; *got one [online](http://www.moodycamel.com/blog/2014/a-fast-general-purpose-lock-free-queue-for-c++)*.
	+ [ ] Render commands basic design w/ shader-pipeline binding.
	+ [x] Implement `generation`s for render resources!
- [ ] Legit `Material` asset. PBR, really.
- [x] Write Alexandria member functions to load assets (no OOP).
- [x] Use `apathy::Path` for everything in Alexandria.
- [x] Figure out how Alexandria deals with loading assets. Should assets be PODs (mostly)?
- [ ] `MessagePack` for `RenderCommand`s seems like a decent idea. After all, we'd have to implement something similar ourselves.
- [ ] Multi-thread `Alexandria` asset loading to avoid loading hell at game launch. Task-based? That would be nice.
- [ ] Memory pool. We need it in many places at this point, and it will only get worse. Sparse memory is a pain.
- [ ] Start thinking about a minimal Lua binding interface (use `talloc`)
	+ [Think twice before eating all that glue](http://purplepwny.com/blog/binding_lua_to_c_think_twice_before_eating_that_glue.html).
	+ [Lightweight Lua bindings (Bitsquid)](http://bitsquid.blogspot.com.es/2011/06/lightweight-lua-bindings.html).
- [ ] Basic `Bullet` physics integration.
- [ ] Skeletal animation.
- [ ] Shader builder basic design.

## Kinda out there
- [ ] Deal with asset dependencies: `unload`ing an asset should somehow take into account the ones that depend on it. In other words, track dependencies and be mindful of them.
- [ ] Better `Asset` packing (beware of prematurely optimizing). Start working on the toolchain.

## On the horizon
- `Sys.Script` + hooks