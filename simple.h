#ifndef _SIMPLE_
#define _SIMPLE_

// find out len of __VA_ARGS__ with __VA_NARG__(__VA_ARGS__)
#define __VA_NARG__(...) (__VA_NARG_(_0, ## __VA_ARGS__, __RSEQ_N()) - 1)
#define __VA_NARG_(...) __VA_ARG_N(__VA_ARGS__)
#define __VA_ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, _61,_62,_63,N,...) N
#define __RSEQ_N() 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#ifdef _SIMPLE_HASHMAP_IMPLEMENTATION
#define _SIMPLE_VECTOR_IMPLEMENTATION
#endif

#ifndef reallocarray
#include <assert.h>
#include <malloc.h>
#include <sys/types.h>
void *reallocarray(void *olddata, size_t itemsize, size_t itemcount) {
	assert((itemsize&itemcount) == 0 || (itemsize * itemcount) / itemsize == itemcount);
	return realloc(olddata, itemsize * itemcount);
}
#endif



#ifndef vector
#include <sys/types.h>

typedef struct {
	size_t head; // amount of items stored
	size_t size; // cap of items
	void **data; // sizeof(data) = size*sizeof(void*)
} vector;
#endif // vector def

vector *vec_make(size_t preloc_size, size_t used_size, void *data);
void vec_free(vector *vec);
int vec_push(vector *vec, void *data); // -1 on error, index on OK
int vec_set(vector *vec, void *data, size_t index);
int vec_size(vector *vec);
int vec_cap(vector *vec);
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

#include <assert.h>

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
	return vec->head++;
}

int vec_size(vector *vec) {
	return vec->head-1;
}

int vec_cap(vector *vec) {
	return vec->size;
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

#ifndef dlist
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
#endif

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
	struct _dnode *nd =	dlist_get_node(dl, index);
	if (nd == NULL) {return -1;}

	if (dl->do_gc) {
		free(nd->obj);
	}

	nd->obj = obj;
	return 0;
}

int dlist_remove(dlist *dl, size_t index) {
	if (dl->len == 0 || index >= dl->len) {
		errno = ERANGE;
		return -1;
	}
	struct _dnode *removed =	dlist_get_node(dl, index);
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
		struct _hashnode *hnode = vec_get(nlist, i);
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
		if (!found) {
			struct _hashnode *hnode = vec_get(nlist, i);
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

#ifdef _SIMPLE_ARRAY_IMPLEMENTATION

#ifndef size_t
#include <sys/types.h>
#endif

typedef  struct {
	size_t cap;
	size_t item_size;
} array_head;

#define array_get_head(T) (((void*)T)-sizeof(array_head))
#define array_get_size(T) (array_get_head(T)->cap)
#define _array_used_bytes(head) (head->item_size*head->cap+sizeof(array_head))
#define array_used_bytes(T) (_array_used_bytes(array_get_head(T)))
#define array_free(T) (free(array_get_head(T)))

#define array_make(T, preloc) ((T*)_array_make(sizeof(T), preloc))
void *_array_make(size_t item_size, size_t preloc) {
	void *data = malloc(sizeof(array_head)+(item_size*preloc));
	if (data == NULL) return NULL;

	array_head* head = ((array_head*)data);
	void *array_data = (data+sizeof(array_head));

	head->cap = preloc;
	head->item_size = item_size;
	return array_data;
}

#define array_resize(T, size) ((__typeof__(T))_array_resize((void*)T, size))
void *_array_resize(void *data, size_t size) {
	array_head *head = array_get_head(data);
	head->cap = size;
	void *ndata = reallocarray(head, 1, _array_used_bytes(head));
	if (ndata == NULL) return NULL;
	return ndata+sizeof(*head);
}

#define array_minsize(T, size) (__typeof__(T))_array_minsize((void*)T, size)
void *_array_minsize(void *data, size_t minsize) {
	array_head *head = array_get_head(data);
	if (head->cap >= minsize) return data;
	head->cap = head->cap*2;
	void *ndata = reallocarray(head, 1, (head->item_size*head->cap)+sizeof(*head));
	if (ndata == NULL) return NULL;
	return ndata+sizeof(*head);
}

#define _fn(T, v, index, fn) (fn( ((T)v), index))
#define itert_fn(T, index, fn) _fn(__typeof__(*T), T[index], index, fn)
#define array_forEach(data, fn) do { \
	array_head *head = array_get_head(data); \
	for (int i = 0; i<head->cap;i++) { \
		itert_fn(data, i, fn); \
	} \
} while (0)

#endif // _SIMPLE_ARRAY_IMPLEMENTATION

#endif // _SIMPLE_

