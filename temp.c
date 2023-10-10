#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define _SIMPLE_VECTOR_IMPLEMENTATION
#include "simple.h"

// f = c*1.8+32
// c = (f-32)/1.8

// k = c+273.15
// c = k-273.15

float from_kelvin(float k) {
	return k-273.15;
}
float to_kelvin(float c) {
	return c+273.15;
}

float from_fahrenheit(float f) {
	return (f-32)/1.8;
}
float to_fahrenheit(float c) {
	return c*1.8+32;
}

float get_celsius(const char *_text) {
	char text[strlen(_text)+1];
	strcpy(text, _text);
	char tp = text[strlen(text)-1];
	char *num = strtok(text, "kfcKFC°");
	// _text only has "kfcKFC°" chars
	if (num == NULL) return 0.0;

	float val = atof(num);
	if (tp == 'c' || tp == 'C') {
		return val;
	} else if (tp == 'k' || tp == 'K') {
		return from_kelvin(val);
	} else if (tp == 'f' || tp == 'F') {
		return from_fahrenheit(val);
	} else if (tp == num[strlen(num)-1]) { // it's celsius
		return val;
	}
	fprintf(stderr, "temperature is specified, but unit must K, F or C; got: '%c'\n", tp);
	return 0.0;
}

int main(int argc, char **argv) {
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

	// should print "to <unit>:" ?
	if (kelvin->head) {
		printf("to kelvin:\n");
		while (kelvin->head) {
			char *text = (char*)vec_pop(kelvin);
			float num = get_celsius(text);
			printf("%s -> %.3fK\n", text, to_kelvin(num));
		}
	}

	if (fahrenheit->head) {
		printf("to fahrenheit:\n");
		while (fahrenheit->head) {
			char *text = (char*)vec_pop(fahrenheit);
			float num = get_celsius(text);
			printf("%s -> %.3f°F\n", text, to_fahrenheit(num));
		}
	}

	if (celsius->head) {
		printf("to celsius:\n");
		while (celsius->head) {
			char *text = (char*)vec_pop(celsius);
			float num = get_celsius(text);
			printf("%s -> %.3f°C\n", text, num);
		}
	}

	vec_free(kelvin);
	vec_free(fahrenheit);
	vec_free(celsius);
}

