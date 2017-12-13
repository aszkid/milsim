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
- [x] Legit `Material` asset.
	+ [ ] Test it!
- [ ] Put `Alexandria` loading snippets in corresponding asset classes!
- [ ] Start thinking about a minimal Lua binding interface (use `talloc`)
	+ [Think twice before eating all that glue](http://purplepwny.com/blog/binding_lua_to_c_think_twice_before_eating_that_glue.html).
	+ [Lightweight Lua bindings (Bitsquid)](http://bitsquid.blogspot.com.es/2011/06/lightweight-lua-bindings.html).
- [ ] Basic `Bullet` physics integration.
- [ ] Skeletal animation.

## Kinda out there
- [ ] Look up `std::multimap` for faster `Hermes`? Of better design in general.
- [ ] Multi-thread `Alexandria` asset loading to avoid loading hell at game launch
- [ ] Better `Asset` packing (beware of prematurely optimizing). Start working on the toolchain.

## On the horizon
- `Sys.Script` + hooks