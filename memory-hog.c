#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 31337 * 4096

struct item {
    char stuff[SIZE];
    struct item * next;
};

struct cache_items {
    struct item * head;
    struct item * tail;
    pthread_mutex_t lock;
} cache;

void hog_memory() {
    struct item * thing;
    thing = malloc(sizeof(struct item));

    pthread_mutex_lock(&cache.lock);
    if (cache.tail) {
        cache.tail->next = thing;
        cache.tail = thing;
    } else {
        cache.head = thing;
        cache.tail = thing;
    }
    pthread_mutex_unlock(&cache.lock);

    printf("Oink!\n");
    sleep(1);
}

extern void *osv_register_shrinker(const char *, size_t (*)(size_t, bool));

// TODO: Add an example where the hard parameter is taken into account.
size_t shrinker_function(size_t target, bool hard)
{
    size_t freed = 0;
    struct item * thing;
    printf("shrinker: processing request to free %08d bytes.\n", target);
    if (!hard) {
        printf("Soft pressure, all done.\n");
        return 0;
    }

    pthread_mutex_lock(&cache.lock);
    thing = cache.head;
    while (thing != NULL && freed <= target) {
        cache.head = thing->next;
        free(thing);
        freed += sizeof(struct item);
        thing = cache.head;
    }
    pthread_mutex_unlock(&cache.lock);

    printf("shrinker: %08d bytes of memory were freed!\n", freed);
    return freed;
}

int main (int argc, int **argv) {
    // Initialize cache
    cache.head = NULL;
    cache.tail = NULL;
    pthread_mutex_init(&cache.lock, NULL);

    printf("I'm a memory hog!\n");
    osv_register_shrinker("Example Shrinker", shrinker_function);
    for (;;) {
        hog_memory();
    }
}
