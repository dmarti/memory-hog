Using the OSv shrinker API, with sample code
============================================

If you're writing a program that keeps a cache, you're probably expecting users to have to set the cache size, which means a configuration setting or command-line argument.  And every configuration setting or command-line argument is something that you have to document, or explain to users when they get it wrong.

Thankfully, there's an easier way.  With OSv, you can ask the OS to let your program know when memory is tight, so that you can manage the cache size on the fly.  Less time spent tweaking settings, more items kept in cache, what's not to like?  Just set up a shrinker callback and register it.  (There is a [mechanism for memory pressure notifications](https://www.kernel.org/doc/Documentation/cgroups/memory.txt) on Linux, but it's somewhat complex. With OSv it's just one function to write.)

Defining a shrinker function
----------------------------

OSv notifies your program of a low memory situation by
calling a shrinker function.

A shrinker function takes two arguments:

 * target amount of memory to free (size_t)

 * A boolean "hard" argument.  This is false if the function is being called for preemptive freeing of memory, and true if the system is under severe pressure.


Registering
-----------

Now that the shrinker function is defined, you need to register it.

To register a shrinker function involves calling `osv_register_shrinker`:

```
void *osv_register_shrinker(const char *name,
                            size_t (*func)(size_t target, bool hard));
```

For example, an application in C can just do:

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

An extremely basic example of a program that uses the OSv shrinker API is [memory-hog](https://github.com/dmarti/memory-hog).  To try it, install [Capstan](https://github.com/cloudius-systems/capstan), clone the Git repository, and build an OSv image to run it.  Currently memory-hog requires at least 2GB of memory.


```
$ git clone git@github.com:dmarti/memory-hog.git
Cloning into 'memory-hog'...
remote: Reusing existing pack: 76, done.
remote: Total 76 (delta 0), reused 0 (delta 0)
Receiving objects: 100% (76/76), 12.84 KiB | 0 bytes/s, done.
Resolving deltas: 100% (39/39), done.
Checking connectivity... done.
$ cd memory-hog
$ capstan run -m 2G
Building memory-hog...
Created instance: memory-hog
OSv v0.09
eth0: 192.168.122.15
I'm a memory hog!
program: 	Oink!
program: 	Oink!
program: 	Oink!

...many "Oink!"s later...

program: 	Oink!
shrinker:	Soft pressure, all done.
program: 	Oink!
shrinker:	Soft pressure, all done.
program: 	Oink!
shrinker:	Soft pressure, all done.
program: 	Oink!
shrinker:	Soft pressure, all done.
program: 	Oink!
shrinker:	Soft pressure, all done.
program: 	Oink!
shrinker:	Soft pressure, all done.
shrinker:	processing request to free 166062080 bytes.
shrinker:	starting with 64 things.
shrinker:	finishing with 58 things.
		192534576 bytes of memory were freed!
program: 	Oink!
shrinker:	Soft pressure, all done.
```


Concurrency
-----------

Shrinker callbacks conceptually work like asynchronous signal handlers,
They handle specific events and may be called at any time, from your program's point of view.

Each cache in your program that the shrinker can release data from,
must have its own lock to prevent concurrency issues.

The memory-hog sample program uses a mutex to handle concurrency.  Another good option is to simply keep a global `free_memory_please` global variable, and set it in the shrinker callback.  Then check it in the main loop, and free memory if necessary.


Handling soft pressure
----------------------

If possible, you should have your shrinker try to release some memory when called with hard=false, and free some extra memory when hard=true.  This will reduce the number of times you have to handle shrinking, and improve performance by making more free memory available to OSv for system tasks.  The memory-hog example currently ignores soft pressure.

For more info on memory management and other OSv subjects, please join the [osv-dev mailing list](https://groups.google.com/forum/#!forum/osv-dev).  You can get updates on by subscribing to the [OSv blog RSS feed](http://osv.io/blog/atom.xml) or folllowing [@CloudiusSystems](https://twitter.com/CloudiusSystems) on Twitter.

