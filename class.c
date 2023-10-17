
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

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define UINT_MAX 0xFFFFFFFF
#define _SIMPLE_HASHMAP_IMPLEMENTATION
#define _SIMPLE_FNVHASH_IMPLEMENTATION
#include "simple.h"
#define fstr(str) str, strlen(str)

typedef struct {
	hashmap *vmap;
} callable;

// maybe get variatic arguments
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

typedef struct {
	hashmap *_vtable;
	char *filename;
	int fd;
	int status;
	struct stat st_status;
	char *read_buffer;
} file;

#define FILE_STATUS_OPEN_FLAG 1
#define FILE_STATUS_STAT_FLAG 2
#define FILE_STATUS_READ_FLAG 4

void *make_generic(size_t size, hashmap *vtable) {
	void *struc = malloc(size);
	((callable*)struc)->vmap = vtable;
	return struc;
}

hashmap *people_vtable;
person *make_person() {
	return make_generic(sizeof(person), people_vtable);
}

hashmap *file_vtable;
file *make_file(char *filename) {
	file *f = make_generic(sizeof(file), file_vtable);
	f->filename = filename;
	return f;
}

void file_open(file *f) {
	f->fd = open(f->filename, O_RDONLY);
	f->status |= FILE_STATUS_OPEN_FLAG;
}

#define not_mask(flag) UINT_MAX^(flag)

void file_close(file *f) {
	close(f->fd);
	f->status &= not_mask(FILE_STATUS_OPEN_FLAG);
}

void *file_stat(file *f) {
	if (f->status & FILE_STATUS_OPEN_FLAG) {
		fstat(f->fd, &f->st_status);
	} else {
		stat(f->filename, &f->st_status);
	}
	f->status |= FILE_STATUS_STAT_FLAG;
	return &f->st_status;
}

void *file_read(file *f) {
	if (! (f->status & FILE_STATUS_OPEN_FLAG)) {
		call(f, "open");
	}

	if (! (f->status & FILE_STATUS_STAT_FLAG)) {
		call(f, "stat");
	}

	f->read_buffer = malloc(f->st_status.st_size);
	f->status |= FILE_STATUS_READ_FLAG;
	assert(read(f->fd, f->read_buffer, f->st_status.st_size));
	return f->read_buffer;
}

void file_free(file *f) {
	if ( f->status &= FILE_STATUS_READ_FLAG ) {
		free(f->read_buffer);
	}
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
void *person_getage(person *p) {
	return (void*)&p->age;
}

int main() {
	people_vtable = map_make(4, 0, fnv_wrapper);
	file_vtable = map_make(4, 0, fnv_wrapper);
	map_add(people_vtable, fstr("print"), (void*)person_print);
	map_add(people_vtable, fstr("age"), (void*)person_age);
	map_add(people_vtable, fstr("get age"), (void*)person_getage);
	map_add(file_vtable, fstr("open"), (void*)file_open);
	map_add(file_vtable, fstr("close"), (void*)file_close);
	map_add(file_vtable, fstr("stat"), (void*)file_stat);
	map_add(file_vtable, fstr("free"), (void*)file_free);
	map_add(file_vtable, fstr("read"), (void*)file_read);

	person *p1 = make_person();
	file *f1 = make_file("class.c");

	struct stat *csts = (struct stat *)call(f1, "stat");
	printf("stated (closed file)!: %p\n", (void*)csts);
	printf("fsize: %ld\n", csts->st_size);

	call(f1, "open");
	printf("file status: %s\n", f1->status&1?"open":"closed");

	char *txt = call(f1, "read");
	printf("%ld\n", strlen(txt));

	call(f1, "close");
	printf("file status: %s\n", f1->status&1?"open":"closed");

	p1->age = 10;
	p1->id = 1;
	call(p1, "print");
	call(p1, "age");
	call(p1, "print");

	map_free(people_vtable);
	map_free(file_vtable);
	free(p1);
	free(f1);
}
