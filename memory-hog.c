#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define SIZE 31337 * 4096

struct item {
    char stuff[SIZE];
    struct item * next;
};

struct item * head;
struct item * tail;

void hog_memory() {
    struct item * thing;
    thing = malloc(sizeof(struct item));
    if (tail) {
        tail->next = thing;
        tail = thing;
    } else {
        head = thing;
        tail = thing;
    }
    printf("Oink!\n");
    sleep(1);
}

extern void *osv_register_shrinker(const char *, size_t (*)(size_t, bool));

// TODO: Add an example where the hard parameter is taken into account.
size_t shrinker_function(size_t target, bool hard)
{
    size_t freed = 0;
    struct item * thing = head;
    printf("shrinker: processing request to free %08d bytes.\n", target);
    while (thing != NULL && freed <= target) {
        head = thing->next;
        free(thing);
        freed += sizeof(struct item);
        thing = head;
    }
    printf("shrinker: %08d bytes of memory were freed!\n", freed);
    return freed;
}

int main (int argc, int **argv) {
    head = NULL;
    tail = NULL;
    printf("I'm a memory hog!\n");
    osv_register_shrinker("Example Shrinker", shrinker_function);
    for(;;) {
        hog_memory();
    }
}
