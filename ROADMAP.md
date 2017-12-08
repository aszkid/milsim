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
	+ [ ] Thread-safe `queue` implementation for Main-Render threads communication.
- [ ] Legit `Material` asset.
- [ ] Basic `Bullet` physics integration.
- [ ] Skeletal animation.

- [ ] Multi-thread `Alexandria` asset loading to avoid loading hell at game launch
- [ ] Better `Asset` packing (beware of prematurely optimizing). Start working on the toolchain.

## Kinda out there
- [ ] Look up `std::multimap` for faster `Hermes`?

## On the horizon
- `Sys.Script` + hooks