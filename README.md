memory-hog
==========

Hey, kids, let's learn OSv's shrinker API!

If you're writing a program that keeps a cache,
you're probaly expecting users to have to set the
cache size, which means a configuration setting
or command-line argument.  And every configuration
setting or command-line argument is something that
you have to document, or explain to users when they
get it wrong.


A shrinker function takes two arguments:

 * target amount of memory to free (size_t)

 * "hard" -- false if this is a preemptive freeing of
   memory, true if the system is under severe
   pressure.


For C applications, or C++ applications that are built without access to OSv headers, registering a shrinker involves calling the following function:

'''
void *osv_register_shrinker(const char *name,
                            size_t (*func)(size_t target, bool hard));
'''

An application in C can just do:

'''
extern void *osv_register_shrinker(const char *name,
                            size_t (*func)(size_t target, bool hard));

int main () {
  ...
  osv_register_shrinker("Example Shrinker", shrinker_function);
  ...
  return 0;
}
'''

