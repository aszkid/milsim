# Questions

Various questions and issues.

+ The `Input` system favours indirect handling: every interested component simply has to subscribe to some input-like event to get input messages. This is fine if everything works perfectly well and no messages are dropped. If an `Object` interested in input is created *after*, say, we have started pressing a key, there is no way for it to learn about that, until we release and press the key again. Plus, an object might receive the message, but not want to act upon receival, rather wait some amount of frames/time. There are more situations like these that favour a *polling-like* system for input handling, i.e. `m_input->get_key(key::W)` instead of checking through messages. Think about this...