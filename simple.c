#ifndef _SIMPLE_C_
#define _SIMPLE_C_

typedef struct {
	int head; // amount of items stored
	int size; // cap of items
	void **data; // sizeof(data) = size*sizeof(void*)
} vector;

// recomended functions
vector *vec_make(int preloc_size, int used_size, void *data);
int vec_push(vector *vec, void *data);
void *vec_pop(vector *vec);
void vec_free(vector *vec); // *

// internal, more specific, functions
void *_vec_get(vector *vec, int index);
int _vec_double(vector *vec);
int _vec_insert(vector *vec, void *data, int index);
int _vec_sethead(vector *vec, int newhead);

// *vec_free doesn't free() bytes inside **data
#ifdef _SIMPLE_C_VECTOR_IMPLEMENTATION
vector *vec_make(int preloc_size, int used_size, void *data) {
	if (preloc_size == 0) {
		assert(data == NULL); // can't init vector with data and size = 0
		preloc_size = 1;
	}

	if (data == NULL) {
		data = calloc(sizeof(void*), preloc_size);
	}
	vector *vec = malloc(sizeof(vector));
	vec->size = preloc_size;
	vec->head = used_size;
	vec->data = data;
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
		return NULL;
	}
	return vec->data[index];
}

void *vec_pop(vector *vec) {
	if (vec->head == 0) {
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

#endif // _SIMPLE_C_
