#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct {
	int head; // amount of items stored
	int size; // cap of items
	void **data; // sizeof(data) = size*sizeof(void*)
} vector;

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

int vec_sethead(vector *vec, int newhead) {
	while (newhead >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->head = newhead;
	return 0;
}

// sets head to be index+1
int vec_insert(vector *vec, void *data, int index) {
	while (index >= vec->size) {
		if (_vec_double(vec)) {
			return -1;
		}
	}
	vec->data[index] = data;
	vec->head = index+1;
	return 0;
}

void *vec_get(vector *vec, int index) {
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

int main(int argc, char** argv) {
	vector *kelvin = vec_make(0, 0, NULL);
	vector *fahrenheit = vec_make(0, 0, NULL);
	vector *celsius = vec_make(0, 0, NULL);
	vector *current = celsius;

	for (int i = 1; i<argc; i++) {
		if (!strncmp(argv[i], "-k", 2)) {
			current = kelvin;
		} else if (!strncmp(argv[i], "-f", 2)) {
			current = fahrenheit;
		} else if (!strncmp(argv[i], "-c", 2)) {
			current = celsius;
		} else {
			vec_push(current, argv[i]);
		}
	}

	if (kelvin->head) {
		printf("to kelvin:\n");
		while (kelvin->head) {
			printf("%s\n", (char*)vec_pop(kelvin));
		}
	}

	if (fahrenheit->head) {
		printf("to fahrenheit:\n");
		while (fahrenheit->head) {
			printf("%s\n", (char*)vec_pop(fahrenheit));
		}
	}

	if (celsius->head) {
		printf("to celsius:\n");
		while (celsius->head) {
			printf("%s\n", (char*)vec_pop(celsius));
		}
	}

	vec_free(kelvin);
	vec_free(fahrenheit);
	vec_free(celsius);
}

