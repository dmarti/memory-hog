#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 31337 * 1024

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
    struct item * thing = malloc(sizeof(struct item));

    pthread_mutex_lock(&cache.lock);
    if (cache.tail) {
        cache.tail->next = thing;
        cache.tail = thing;
    } else {
        cache.head = thing;
        cache.tail = thing;
    }
    printf("program: \tOink!\n");
    pthread_mutex_unlock(&cache.lock);
    sleep(1);
}

int thing_count(struct item * thing) {
    if (NULL == thing) {
        return 0;
    }
    return 1 + thing_count(thing->next);
}

extern void *osv_register_shrinker(const char *, size_t (*)(size_t, bool));

size_t shrinker_function(size_t target, bool hard)
{
    size_t freed = 0;
    struct item * thing;
    pthread_mutex_lock(&cache.lock);
    if (hard == false) {
        printf("shrinker:\tSoft pressure, all done.\n");
        return 0;
    }
    printf("shrinker:\tprocessing request to free %08d bytes.\n", target);
    printf("shrinker:\tstarting with %d things.\n", thing_count(cache.head));
    thing = cache.head;
    while (freed <= target) {
        cache.head = thing->next;
        free(thing);
        freed += sizeof(struct item);
        thing = cache.head;
    }
    printf("\n");
    printf("shrinker:\tfinishing with %d things.\n", thing_count(cache.head));
    printf("\t\t%08d bytes of memory were freed!\n", freed);
    pthread_mutex_unlock(&cache.lock);
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
