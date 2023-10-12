#ifndef _SIMPLE_
#define _SIMPLE_

#include <sys/types.h>
// vector

typedef struct {
	int head; // amount of items stored
	int size; // cap of items
	void **data; // sizeof(data) = size*sizeof(void*)
} vector;

vector *vec_make(int preloc_size, int used_size, void *data);
void vec_free(vector *vec);
int vec_push(vector *vec, void *data);
void *vec_pop(vector *vec);
void *_vec_get(vector *vec, int index);
int _vec_double(vector *vec);
int _vec_insert(vector *vec, void *data, int index);
int _vec_sethead(vector *vec, int newhead);

// *vec_free doesn't free() bytes inside **data
#ifdef _SIMPLE_VECTOR_IMPLEMENTATION
#include <errno.h>
#include <stdlib.h>
vector *vec_make(int preloc_size, int used_size, void *data) {
	if (preloc_size == 0) {
		assert(data == NULL); // can't init vector with data and size = 0
		preloc_size = 1;
	}

	if (data == NULL) {
		data = calloc(sizeof(void*), preloc_size);
	}
	vector *vec = malloc(sizeof(vector));
	if (vec == NULL) {
		errno = ENOMEM;
	} else {
		vec->size = preloc_size;
		vec->head = used_size;
		vec->data = data;
	}
	return vec;
}

int _vec_double(vector *vec) {
	void *newdata = reallocarray(vec->data, sizeof(void*), vec->size*2);
	if (newdata == NULL) return -1;

	// no need to free after reallocarray
	vec->size = vec->size*2;
	vec->data = newdata;
	return 0;
}

int vec_push(vector *vec, void *data) {
	if (vec->head >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->data[vec->head] = data;
	vec->head++;
	return 0;
}

int _vec_sethead(vector *vec, int newhead) {
	while (newhead >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->head = newhead;
	return 0;
}

// sets head to be index+1
int _vec_insert(vector *vec, void *data, int index) {
	while (index >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->data[index] = data;
	vec->head = index+1;
	return 0;
}

void *_vec_get(vector *vec, int index) {
	if (index > vec->size) {
		errno = ERANGE;
		return NULL;
	}
	return vec->data[index];
}

void *vec_pop(vector *vec) {
	if (vec->head == 0) {
		errno = ERANGE;
		return NULL;
	}
	vec->head--;
	return vec->data[vec->head];
}

void vec_free(vector *vec) {
	free(vec->data);
	free(vec);
}
#endif

// FNV hash

#define FNV_PRIME 0x811C9DC5

#ifdef __cplusplus
#include <string>
unsigned int FNVStringHash(std::string str);
#endif
unsigned int FNVHash(const char *blob, size_t blob_size);

#ifdef _SIMPLE_FNVHASH_IMPLEMENTATION

#ifndef __cplusplus
#include <string.h>
#endif

#ifdef __cplusplus
unsigned int FNVStringHash(std::string str) {
	return FNVHash(str.c_str(), str.length());
}
#else
unsigned int FNVStringHash(char* str) {
	return FNVHash(str, strlen(str));
}
#endif

unsigned int FNVHash(const char *blob, size_t blob_size) {
	unsigned int hash = 0;

	for (int i = 0; i < blob_size; i++) {
		hash *= FNV_PRIME;
		hash ^= blob[i];
	}

	return hash;
}
#endif // FNV Hash implementation

// doubly linked list

struct _dnode {
	struct _dnode *prev;
	struct _dnode *next;
	void *obj;
};

typedef struct {
	struct _dnode *first;
	struct _dnode *last;
	size_t len;
	int do_gc;
} dlist;

dlist *dlist_make(int collect_garbage);
void dlist_free(dlist *dlj);
void dnode_free(struct _dnode *nd, int free_obj);
struct _dnode *dnode_box(void *obj); // {.obj = obj}
void *dlist_get(dlist *dl, size_t index);
struct _dnode *dlist_get_node(dlist *dl, size_t index);
struct _dnode *dlist_traverse_left(dlist *dl, size_t index); // from start->next
struct _dnode *dlist_traverse_right(dlist *dl, size_t index); // from end->prev
struct _dnode *dnode_get_next(struct _dnode *nd);
struct _dnode *dnode_get_prev(struct _dnode *nd);
int dlist_insert_after(dlist *dl, size_t index, void* obj); // [index-1].next = obj
int dlist_insert_before(dlist *dl, size_t index, void* obj); // [index-1].next = obj
int dlist_append(dlist *dl, void *obj); // last.next = obj
int dlist_push(dlist *dl, void *obj); // first.prev = obj
int dlist_set(dlist *dl, size_t index, void *obj); // [index] = obj
int dlist_remove(dlist *dl, size_t index); // [index-1].next = [index+1]

#ifdef _SIMPLE_DLIST_IMPLEMENTATION
#include <errno.h>
#include <stdlib.h>
dlist *dlist_make(int collect_garbage) {
	dlist *dl = malloc(sizeof(dlist));
	if (dl == NULL) {
		errno = ENOMEM;
	} else {
		dl->len = 0;
		dl->first = NULL;
		dl->last = NULL;
		dl->do_gc = collect_garbage;
	}
	return dl;
}

void dlist_free(dlist *dl) {
	struct _dnode *current = dl->first;
	struct _dnode *next = NULL;
	for (size_t i = 0; i < dl->len ; i++) {
		next = current->next;
		dnode_free(current, dl->do_gc);
		current = next;
	}
	free(dl);
}

void dnode_free(struct _dnode *nd, int free_obj) {
	if (free_obj) {
		free(nd->obj);
	}
	free(nd);
}

struct _dnode *dnode_box(void *obj) {
	struct _dnode *nd = malloc(sizeof(struct _dnode));
	if (nd == NULL) {
		errno = ENOMEM;
	} else {
		nd->obj = obj;
		nd->next = NULL;
		nd->prev = NULL;
	}
	return nd;
}

void *dlist_get(dlist *dl, size_t index){
	struct _dnode *box = dlist_get_node(dl, index);
	if (box != NULL) {
		return box->obj;
	} else {
		return NULL;
	}
}

struct _dnode *dlist_get_node(dlist *dl, size_t index){
	if (index >= dl->len) {
		errno = ERANGE;
		return NULL;
	}

	if (index > (dl->len/2)) {
		return dlist_traverse_right(dl, index);
	} else {
		return dlist_traverse_left(dl, index);
	}
}

struct _dnode *dlist_traverse_right(dlist *dl, size_t index) {
	struct _dnode *current = dl->last;
	for (size_t i = dl->len-1; i > index ; i--) {
		current = current->prev;
	}
	return current;
}

struct _dnode *dlist_traverse_left(dlist *dl, size_t index) {
	struct _dnode *current = dl->first;
	for (size_t i = 0; i < index ; i++) {
		current = current->next;
	}
	return current;
}

struct _dnode *dnode_get_next(struct _dnode *nd){
	if (nd->next == NULL) {
		errno = ERANGE;
	}
	return nd->next;
}

struct _dnode *dnode_get_prev(struct _dnode *nd){
	if (nd->prev == NULL) {
		errno = ERANGE;
	}
	return nd->prev;
}

int dlist_insert_after(dlist *dl, size_t index, void* obj) {
	if (index == dl->len-1) {
		dlist_append(dl, obj);
	}

	if (index >= dl->len) {
		errno = ERANGE;
		return -1;
	}

	struct _dnode *boxed = dnode_box(obj);
	if (boxed == NULL) {return -1;}
	errno = 0;
	struct _dnode *prev = dlist_get_node(dl, index);
	if (prev == NULL && errno == ERANGE) {return -1;}
	boxed->prev = prev;
	boxed->next = prev->next;
	boxed->next->prev = boxed;
	prev->next = boxed;
	dl->len++;
	return 0;
}

int dlist_insert_before(dlist *dl, size_t index, void* obj) {
	if (index == 0) {
		return dlist_push(dl, obj);
	}

	if (index >= dl->len) {
		errno = ERANGE;
		return -1;
	}

	struct _dnode *boxed = dnode_box(obj);
	if (boxed == NULL) {return -1;}
	errno = 0;
	struct _dnode *next = dlist_get_node(dl, index);
	if (next == NULL && errno == ERANGE) {return -1;}
	boxed->prev = next->prev;
	boxed->next = next;
	boxed->prev->next = boxed;
	next->prev = boxed;
	dl->len++;
	return 0;
}

int dlist_append(dlist *dl, void *obj){
	struct _dnode *boxed = dnode_box(obj);
	if (boxed == NULL) {return -1;}
	dl->len++;

	if (dl->last) {
		dl->last->next = boxed;
		boxed->prev = dl->last;
	}
	if (!dl->first) {
		dl->first = boxed;
	}
	dl->last = boxed;
	return 0;
}

int dlist_push(dlist *dl, void *obj){
	struct _dnode *boxed = dnode_box(obj);
	if (boxed == NULL) {return -1;}
	dl->len++;

	if (dl->first) {
		dl->first->prev = boxed;
		boxed->next = dl->first;
	}
	if (!dl->last) {
		dl->last = boxed;
	}
	dl->first = boxed;
	return 0;
}

int dlist_set(dlist *dl, size_t index, void *obj) {
	if (index >= dl->len) {
		errno = ERANGE;
		return -1;
	}
	struct _dnode *nd =  dlist_get_node(dl, index);
	if (nd == NULL) {return -1;}

	if (dl->do_gc) {
		free(nd->obj);
	}

	nd->obj = obj;
	return 0;
}

int dlist_remove(dlist *dl, size_t index) {
	printf("len: %ld, index: %lu\n", dl->len, index);

	if (dl->len == 0 || index >= dl->len) {
		errno = ERANGE;
		return -1;
	}
	struct _dnode *removed =  dlist_get_node(dl, index);
	struct _dnode *prev = removed->prev;
	struct _dnode *next = removed->next;
	if (prev != NULL && next != NULL) {
		prev->next = next;
		next->prev = prev;
	} else {
		if (dl->len == 1) {
			dl->first = NULL;
			dl->last = NULL;
		} else if (index == 0) {
			next->prev = NULL;
			dl->first = next;
		} else {
			prev->next = NULL;
			dl->last = prev;
		}
	}
	dnode_free(removed, dl->do_gc);
	dl->len--;
	return 0;
}
#endif // _SIMPLE_DLIST_IMPLEMENTATION

#endif // _SIMPLE_
