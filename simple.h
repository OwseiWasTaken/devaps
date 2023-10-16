#ifndef _SIMPLE_
#define _SIMPLE_

#include <string.h>
#define fats(str) str, strlen(str)

#ifdef _SIMPLE_HASHMAP_IMPLEMENTATION
#define _SIMPLE_VECTOR_IMPLEMENTATION
#endif

#include <sys/types.h>
// vector

typedef struct {
	size_t head; // amount of items stored
	size_t size; // cap of items
	void **data; // sizeof(data) = size*sizeof(void*)
} vector;

vector *vec_make(size_t preloc_size, size_t used_size, void *data);
void vec_free(vector *vec);
int vec_push(vector *vec, void *data);
int vec_set(vector *vec, void *data, size_t index);
void *vec_pop(vector *vec);
void *vec_tail(vector *vec);
void *vec_head(vector *vec);
void *vec_get(vector *vec, size_t index);
int _vec_double(vector *vec);
int _vec_sethead(vector *vec, size_t newhead);

// *vec_free doesn't free() bytes inside **data
#ifdef _SIMPLE_VECTOR_IMPLEMENTATION
#include <errno.h>
#include <malloc.h>

//#ifdef reallocarray
//#undef reallocarray
//#endif

#include <assert.h>

#ifndef reallocarray
void *reallocarray(void *olddata, size_t itemsize, size_t itemcount) {
	assert((itemsize&itemcount) == 0 || (itemsize * itemcount) / itemsize == itemcount);
	return realloc(olddata, itemsize * itemcount);
}
#endif

vector *vec_make(size_t preloc_size, size_t used_size, void *data) {
	if (preloc_size == 0) {
		assert(data == NULL); // can't init vector with data and size = 0
		preloc_size = 1;
	}

	if (data == NULL) {
		data = calloc(sizeof(void*), preloc_size);
	}
	vector *vec = malloc(sizeof(vector));
	if (vec != NULL) {
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

int _vec_sethead(vector *vec, size_t newhead) {
	while (newhead >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->head = newhead;
	return 0;
}

// sets head to be index+1
int vec_set(vector *vec, void *data, size_t index) {
	while (index >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->data[index] = data;
	vec->head = index+1;
	return 0;
}

void *vec_get(vector *vec, size_t index) {
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

void *vec_tail(vector *vec) {
	if (vec->head == 0) {
		errno = ERANGE;
		return NULL;
	}
	return vec->data[vec->head-1];
}

void *vec_head(vector *vec) {
	if (vec->head == 0) {
		errno = ERANGE;
		return NULL;
	}
	return vec->data[0];
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
unsigned int FNVHash(const char *blob, size_t blobsize);

#ifdef _SIMPLE_FNVHASH_IMPLEMENTATION

#ifdef __cplusplus
unsigned int FNVStringHash(std::string str) {
	return FNVHash(str.c_str(), str.length());
}
#else
#include <string.h>
unsigned int FNVStringHash(char* str) {
	return FNVHash(str, strlen(str));
}
#endif

unsigned int FNVHash(const char *blob, size_t blobsize) {
	unsigned int hash = 0;

	for (size_t i = 0; i < blobsize; i++) {
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
#include <malloc.h>
dlist *dlist_make(int collect_garbage) {
	dlist *dl = malloc(sizeof(dlist));
	if (dl != NULL) {
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
	if (nd != NULL) {
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


struct _hashnode {
	void *obj;
	char *keyblob;
	size_t blobsize;
};

typedef struct {
	size_t size; // amount of lists in vector
	size_t used; // amount of used nodes in vector
	size_t entries; // sum of
	int do_gc;
	size_t (*HashFunction)(char *keyblob, size_t blobsize);
	vector/*<*_hashnode>*/ **nodes;
} hashmap;

hashmap *map_make(size_t size, int gc, size_t (*HashFunction)(char *keyblob, size_t blobsize));
int map_add(hashmap *map, char *keyblob, size_t blobsize, void *obj);
int map_remove(hashmap *map, char *keyblob, size_t blobsize);
void *map_search(hashmap *map, char *keyblob, size_t blobsize);
void map_free(hashmap *map);

#define _SIMPLE_MAP_FREEKEY 1
#define _SIMPLE_MAP_FREEOBJ 2

#ifdef _SIMPLE_HASHMAP_IMPLEMENTATION

hashmap *map_make(size_t size, int gc, size_t (*HashFunction)(char *keyblob, size_t blobsize)) {
	hashmap *map = malloc(sizeof(hashmap));
	if (map != NULL) {
		map->size = size;
		map->used = 0;
		map->entries = 0;
		map->do_gc = gc;
		map->nodes = calloc(sizeof(vector), size);
		map->HashFunction = HashFunction;
		//TODO vec_2dmake(size) for single calloc
		for (size_t i = 0; i < size; i++) {
			map->nodes[i] = vec_make(0, 0, NULL);
		}
	}
	return map;
}

struct _hashnode *hashnode_box(char *keyblob, size_t blobsize, void *obj) {
	struct _hashnode *hnode = malloc(sizeof(struct _hashnode));
	if (hnode != NULL) {
		hnode->obj = obj;
		hnode->keyblob = keyblob;
		hnode->blobsize = blobsize;
	}
	return hnode;
}

size_t __hash(hashmap *map, char *keyblob, size_t blobsize) {
	return map->HashFunction(keyblob, blobsize);
}

vector/*<*_hashnode>*/ *__map_get_nlist(hashmap *map, char *keyblob, size_t blobsize) {
	size_t hash = __hash(map, keyblob, blobsize);
	return map->nodes[hash%map->size];
}

int map_add(hashmap *map, char *keyblob, size_t blobsize, void *obj) {
	struct _hashnode *hnode = hashnode_box(keyblob, blobsize, obj);
	if (hnode == NULL) {return -1;}
	vector *nlist = __map_get_nlist(map, keyblob, blobsize);
#ifdef NDEBUG
	if (!nlist->head) {
		map->used++;
	}
	map->entries;
#endif
	vec_push(nlist, hnode);
	return 0;
}

void *map_search(hashmap *map, char *keyblob, size_t blobsize) {
	vector *nlist = __map_get_nlist(map, keyblob, blobsize);

	for (size_t i = 0; i < nlist->head; i++) {
		// direct access for easier multi-thread implementation
		struct _hashnode *hnode = nlist->data[i];
		if (hnode->blobsize != blobsize) {continue;}
		if (!memcmp(keyblob, hnode->keyblob, blobsize)) {
			return hnode->obj;
		}
	}
	errno = ERANGE;
	return NULL;
}

void hashnode_free(struct _hashnode *hnode, int do_gc) {
	if (do_gc&_SIMPLE_MAP_FREEOBJ) {
		free(hnode->obj);
	}
	if (do_gc&_SIMPLE_MAP_FREEKEY) {
		free(hnode->keyblob);
	}
	free(hnode);
}

int map_remove(hashmap *map, char *keyblob, size_t blobsize) {
	vector *nlist = __map_get_nlist(map, keyblob, blobsize);

	int found = 0;
	for (size_t i = 0; i < nlist->head; i++) {
		// direct access for easier multi-thread implementation
		if (!found) {
			struct _hashnode *hnode = nlist->data[i];
			if (hnode->blobsize != blobsize) {continue;}
			if (!memcmp(keyblob, hnode->keyblob, blobsize)) {
				found = 1;
				hashnode_free(hnode, map->do_gc);
			}
		} else {
			nlist->data[i-1] = nlist->data[i];
		}
	}
	nlist->head--;
	errno = ERANGE;
	return -1;
}

char *bstr(char *blob, size_t size) {
	char *text = malloc(size*5-1);
	for (size_t i = 0; i < size; i++) {
		snprintf(text+(i*5), 6, "%03hhu, ", blob[i]);
	}
	text[size*5-2] = '\0';
	return text;
}

void map_print(hashmap *map) {
	for (size_t i = 0; i < map->size; i++) {
		vector *nlist = map->nodes[i];
		if (!nlist->head) { continue; }
		printf("vec_%ld [%ld] {\n", i, nlist->head);
		for (size_t k = 0; k < nlist->head; k++) {
			struct _hashnode *hnode = nlist->data[k];
			char *t1 = bstr(hnode->keyblob, hnode->blobsize);
			char *t2 = bstr(hnode->obj, strlen(hnode->obj));
			printf("\t%s: %s\n", t1, t2);
			free(t1);
			free(t2);
		}
		printf("}\n");
	}
}

#define _SIMPLE_MAPPRINTF_FREE_KEYSTR 1
#define _SIMPLE_MAPPRINTF_FREE_OBJSTR 2
void map_printf(hashmap *map,
	char *(*ktostr)(char *keyblob, size_t blobsize),
	char *(*objtostr)(void *obj),
	int freeopt
) {
	for (size_t i = 0; i < map->size; i++) {
		vector *nlist = map->nodes[i];
		if (!nlist->head) {
			printf("vec_%ld [0] {}\n", i);
			continue;
		}
		printf("vec_%ld [%ld] {\n", i, nlist->head);
		for (size_t k = 0; k < nlist->head; k++) {
			struct _hashnode *hnode = nlist->data[k];
			char *kstr = ktostr(hnode->keyblob, hnode->blobsize);
			char *ostr = objtostr(hnode->obj);
			printf("\t%s: %s\n", kstr, ostr);
			if (freeopt & _SIMPLE_MAPPRINTF_FREE_KEYSTR) {
				free(kstr);
			}
			if (freeopt & _SIMPLE_MAPPRINTF_FREE_OBJSTR) {
				free(ostr);
			}
		}
		printf("}\n");
	}
}

void map_free(hashmap *map) {
	for (size_t i = 0; i < map->size; i++) {
		vector *nlist = map->nodes[i];
		for (size_t k = 0; k < nlist->head; k++) {
			hashnode_free(vec_get(nlist, k), map->do_gc);
		}
		vec_free(nlist);
	}
	free(map->nodes);
	free(map);
}

#endif // _SIMPLE_HASHMAP_IMPLEMENTATION

#endif // _SIMPLE_
