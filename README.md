memory-hog
==========

Hey, kids, let's learn OSv's shrinker API!

If you're writing a program that keeps a cache,
you're probably expecting users to have to set the
cache size, which means a configuration setting
or command-line argument.  And every configuration
setting or command-line argument is something that
you have to document, or explain to users when they
get it wrong.

Thankfully, there's an easier way.  With OSv,
you can ask the OS to let your program know
when memory is tight, so that you can manage
the cache size on the fly.  Less settings
tweaking, more items kept in cache, what's not
to like? (There is a [mechanism for memory pressure
notifications](https://www.kernel.org/doc/Documentation/cgroups/memory.txt)
on Linux, but it's somewhat complex.

On OSv, it's a callback model: you just set up a
shrinker callback and register it.


Defining a shrinker function
----------------------------

OSv notifies your program of a low memory situation by
calling a shrinker function.

A shrinker function takes two arguments:

 * target amount of memory to free (size_t)

 * "hard" -- false if this is a preemptive freeing of
   memory, true if the system is under severe
   pressure (bool).


Registering
-----------

Now that the shrinker is defined, you need to register it.

Registering a shrinker involves calling `osv_register_shrinker`:

```
void *osv_register_shrinker(const char *name,
                            size_t (*func)(size_t target, bool hard));
```

An application in C can just do:

```
extern void *osv_register_shrinker(const char *name,
                            size_t (*func)(size_t target, bool hard));

int main () {
  ...
  osv_register_shrinker("Example Shrinker", shrinker_function);
  ...
  return 0;
}
```

Concurrency
-----------

Shrinker callbacks conceptually work like asynchronous signal handlers,
i.e. eventually handling specific events.

Each cache in your program that the shrinker can release data from,
must have its own lock to prevent concurrency issues.

In doubt, check memory-hog code to see how to deal with concurrency.

TODO list
-----------

 * just set a global "target" in the callback, process in main loop?

