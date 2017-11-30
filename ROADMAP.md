# Roadmap

## Immediate Priorities
- [x] Skeleton `Alexandria` implementation.
- [x] Base `Object` class that has a handle to systems.
	+ [ ] Rethink whether `Object` needs a handle to `Alexandria`...

- [ ] Macro for `Objects` to initialize sub-`Objects` in a nice way (i.e. don't have to manually pass `m_hermes`, etc).
- [ ] Simple rendering pipeline implementation (_not_ deferred, calm down boy).
	+ [ ] Task 1
	+ [ ] Task 2
- [ ] `Scene::Node` component-based, i.e. every scene node has a list of `Scene::Component`s?
	+ [ ] Task 3
	+ [ ] Task 4

## Kinda out there
- [ ] Look up `std::multimap` for faster `Hermes`?

## On the horizon
- `Sys.Script` + hooks