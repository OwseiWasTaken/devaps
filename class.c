#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _SIMPLE_FNVHASH_IMPLEMENTATION
#define _SIMPLE_HASHMAP_IMPLEMENTATION
#include "simple.h"

#define fstr(str) str, strlen(str)

typedef struct {
	hashmap *vmap;
} callable;

void *call(void *struc, char *func) {
	hashmap *vmap = ((callable*) struc)->vmap;
	f = map_search(vmap, fstr(func));
	void *(method)(void *struc) = void *(f)(void *struc);
	return method(struc);
}

typedef struct {
	hashmap *_vtable;
	int age;
} person;

void *print_age(void *struc) {
	person *p = (person *)struc;
	printf("%d\n", p->age);
	return NULL;
}

hashmap *people_vtable;
person *make_person() {
	person *p = malloc(sizeof(person));
	p->_vtable = people_vtable;
	return p;
}

size_t fnv_wrapper(char *keyblob, size_t blobsize) {
	return FNVHash(keyblob, blobsize);
}


int main() {
	people_vtable = map_make(4, 0, fnv_wrapper);
	map_add(people_vtable, fstr("print-table"), print_age);
	person *p = make_person();
	p->age = 20;
	call(p, "print_table");

	map_free(people_vtable);
	free(p);
}
