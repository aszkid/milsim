# Roadmap

## Immediate Priorities
- [x] Skeleton `Alexandria` implementation.
- [x] Base `Object` class that has a handle to systems.
	+ [ ] Rethink whether `Object` needs a handle to `Alexandria`...
- [x] Macro for `Objects` to initialize sub-`Objects` in a nice way (i.e. don't have to manually pass `m_hermes`, etc).

- [x] Control camera with mouse
- [ ] "Simple" rendering pipeline implementation (_not_ deferred, calm down boy).
	+ [x] `.obj` loader for Alexandria
	+ [x] Support for textures!
	+ [x] Thread-safe `queue` implementation for Main-Render threads communication; *got one [online](http://www.moodycamel.com/blog/2014/a-fast-general-purpose-lock-free-queue-for-c++)*.
	+ [ ] Pipeline abstraction basic design.
	+ [ ] Render commands basic design.
	+ [ ] Shader builder basic design.
- [x] Legit `Material` asset.
	+ [ ] Test it!
- [x] Write Alexandria member functions to load assets (no OOP).
- [x] Use `apathy::Path` for everything in Alexandria.
- [x] Figure out how Alexandria deals with loading assets. Should assets be PODs (mostly)?
- [ ] `MessagePack` for `RenderCommand`s seems like a decent idea. After all, we'd have to implement something similar ourselves otherwise.
- [ ] Start thinking about a minimal Lua binding interface (use `talloc`)
	+ [Think twice before eating all that glue](http://purplepwny.com/blog/binding_lua_to_c_think_twice_before_eating_that_glue.html).
	+ [Lightweight Lua bindings (Bitsquid)](http://bitsquid.blogspot.com.es/2011/06/lightweight-lua-bindings.html).
- [ ] Basic `Bullet` physics integration.
- [ ] Skeletal animation.

## Kinda out there
- [ ] Look up `std::multimap` for faster `Hermes`? Of better design in general. Thinking about it, (20/12/17) simplify `Hermes`: each producer creates a channel, and consumers can `read_channel` and peek through each message (no copying, just increment a pointer).
- [ ] Deal with asset dependencies: `unload`ing an asset should somehow take into account the ones that depend on it. In other words, track dependencies and be mindful of them.
- [ ] Multi-thread `Alexandria` asset loading to avoid loading hell at game launch
- [ ] Better `Asset` packing (beware of prematurely optimizing). Start working on the toolchain.

## On the horizon
- `Sys.Script` + hooks