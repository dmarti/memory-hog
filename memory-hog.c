#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 31337 * 4096

struct item {
    char stuff[SIZE];
    struct item * prev;
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

/* TODO: Callback function here.  Should free some
 * items. */

int main (int argc, int **argv) {
    head = NULL;
    tail = NULL;
    printf("I'm a memory hog!\n");
    /* TODO: Register a callback here? */
    for(;;) {
        hog_memory();
    }
}
