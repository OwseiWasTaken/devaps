
/*
tring out mutexes
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <wait.h>

#define _SIMPLE_MUTEX_IMPLEMENTATION
#include "simple.h"

int main() {
	futex_t *shared = mmap(
		NULL, sizeof(futex_t) * 3,
		PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_SHARED,
	-1, 0);

	if (shared == MAP_FAILED) {
		perror("failed to allocate shared memory\n");
		exit(EXIT_FAILURE);
	}

	futex_t *f1 = &shared[0];
	futex_t *f2 = &shared[1];
	uint32_t *ccounter = &shared[2];
	*f1 = 0; // unlocked
	*f2 = 1; // locked
	*ccounter = 0; // multi-thread
	int count = 10; // thread specific

	pid_t child_pid = fork();
	if (child_pid) { // parrent
		for (int i = 0; i<count; i++) {
			flock(f1);
			(*ccounter)++;
			printf("[#####] %d::%d {%d, %d}\n", i, *ccounter, *f1, *f2);
			funlock(f2);
		}
	} else { // child
		for (int i = 0; i<count; i++) {
			flock(f2);
			(*ccounter)++;
			printf("[     ] %d::%d {%d, %d}\n", i, *ccounter, *f1, *f2);
			funlock(f1);
		}
	}
	wait(NULL);
}
*/

// class-like vtable to enable methods in structs

#include <stdlib.h>
#include <string.h>
#define _SIMPLE_HASHMAP_IMPLEMENTATION
#define _SIMPLE_FNVHASH_IMPLEMENTATION
#include "simple.h"
#define fstr(str) str, strlen(str)

typedef struct {
	hashmap *vmap;
} callable;


void *call(void *struc, char *func) {
	hashmap *vmap = ((callable*) struc)->vmap;
	void *(*method)(void *struc) = map_search(vmap, fstr(func));
	if (method == NULL) {
		printf("method \"%s\" doesn't exist in such struct\n", func);
		return NULL;
	} else {
		return method(struc);
	}
}

typedef struct {
	hashmap *_vtable;
	int age;
	int id;
} person;

hashmap *people_vtable;
person *make_person() {
	person *p = malloc(sizeof(person));
	p->_vtable = people_vtable;
	return p;
}

size_t fnv_wrapper(char *keyblob, size_t blobsize) {
	return FNVHash(keyblob, blobsize);
}

void person_print(person *p) {
	printf("person[%d] is a %dyo\n", p->id, p->age);
}
void person_age(person *p) {
	p->age++;
}

int main() {
	people_vtable = map_make(4, 0, fnv_wrapper);
	map_add(people_vtable, fstr("print"), (void*)person_print);
	map_add(people_vtable, fstr("age"), (void*)person_age);

	person *p1 = make_person();
	person *p2 = make_person();

	p1->age = 10;
	p1->id = 1;
	p2->age = 20;
	p2->id = 2;
	// p1.print_table() -> call(p1, "print");
	call(p1, "print");
	call(p1, "age");
	call(p1, "print");

	map_free(people_vtable);
	free(p1);
	free(p2);
}
