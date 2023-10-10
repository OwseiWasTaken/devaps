#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define _SIMPLE_VECTOR_IMPLEMENTATION
#include "simple.h"

#ifndef TAPE_SIZE
#define TAPE_SIZE 1024
#endif

#define orange "\x1b[38;2;255;127;0m"
#define nc "\x1b[0m"
#define upline "\x1b[1A"

// []<>+-,.

typedef enum {
	StartLoop = 1,
	EndLoop = 2,
	HeadLeft = 3,
	HeadRight = 4,
	Increment = 5,
	Decrement = 6,
	GetKey = 7,
	PrintKey = 8,
} bf_op_TYPE ;

typedef struct {
	bf_op_TYPE type;
	size_t index; // relevant index for loops
} bf_op;

char type_to_char(bf_op_TYPE op) {
	switch(op) {
		case StartLoop:
			return '[';
		case EndLoop:
			return ']';
		case HeadLeft:
			return '<';
		case HeadRight:
			return '>';
		case Increment:
			return '+';
		case Decrement:
			return '-';
		case GetKey:
			return ',';
		case PrintKey:
			return '.';
	}
	return 0;
}

bf_op_TYPE char_to_type(char op) {
	switch(op) {
		case '[':
			return StartLoop;
		case ']':
			return EndLoop;
		case '<':
			return HeadLeft;
		case '>':
			return HeadRight;
		case '+':
			return Increment;
		case '-':
			return Decrement;
		case ',':
			return GetKey;
		case '.':
			return PrintKey;
	}
	return 0;
}

int main(const int argc, const char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <file.bf>\n", argv[0]);
		return -1;
	}

	const char* bf_filename = argv[1];
	struct stat statbuf;
	stat(bf_filename, &statbuf);
	char *bf_filebuffer = malloc(statbuf.st_size);
	const int bf_fd = open(bf_filename, O_RDONLY);
	const size_t doneread = read(bf_fd, bf_filebuffer, statbuf.st_size);
	close(bf_fd);
	assert(doneread == statbuf.st_size);

	size_t bf_size = 0;
	for (size_t i = 0; i<doneread; i++) {
		if (char_to_type(bf_filebuffer[i])) bf_size++;
	}

	bf_op_TYPE bf_typebuffer[bf_size];
	memset(bf_typebuffer, 0, bf_size);
	for (size_t i = 0, bf_size_i = 0 ; i<doneread; i++) {
		bf_op_TYPE op = char_to_type(bf_filebuffer[i]);
		if (op != 0) {
			bf_typebuffer[bf_size_i] = op;
			bf_size_i++;
		}
	}

	bf_op bf_buffer[bf_size];
	char bf_text_buffer[bf_size];
	vector *loops = vec_make(0, 0, NULL);
	for (size_t i = 0; i<bf_size; i++) {
		bf_buffer[i].type = bf_typebuffer[i];
		bf_text_buffer[i] = type_to_char(bf_typebuffer[i]);

		switch(bf_typebuffer[i]) {
		case StartLoop:
			vec_push(loops, (void*)i);
			break;
		case EndLoop:
			size_t back_index = (size_t) vec_pop(loops);
			if (back_index == 0) assert(errno != ERANGE); // no matching '['
			bf_buffer[back_index].index = i;
			bf_buffer[i].index = back_index;
			break;
		}
	}
	assert(vec_pop(loops) == NULL && errno == ERANGE); // no matching ']'
	vec_free(loops);

	bf_op cop;
	size_t head = 0;
	char tape[TAPE_SIZE] = {0};
	//char _;
	for (size_t i = 0; i<bf_size; i++) {
		// make actual debug 'menu'
		//read(0, &_, 1);
		//system("clear");
		//printf("\x1b[%ldD%s%c", bf_size-i-1, nc, bf_text_buffer[i-1]);
		//printf("%s", bf_text_buffer);
		//printf("\x1b[%ldD%s%c%s", bf_size-i, orange, bf_text_buffer[i], nc);
		//fflush(stdout);
		cop = bf_buffer[i];
		switch(cop.type) {
		case StartLoop:
			if (!tape[head]) {
				i = cop.index;
			}
			break;
		case EndLoop:
			if (!tape[head]) {
				i = cop.index;
			}
			break;
		case HeadLeft:
			assert(head > 0);
			head--;
			break;
		case HeadRight:
			assert(head < TAPE_SIZE);
			head++;
			break;
		case Increment:
			tape[head]++;
			break;
		case Decrement:
			tape[head]--;
			break;
		case GetKey:
			tape[head] = 69;
			break;
		case PrintKey:
			fputc(tape[head], stdout);
			break;
		}
	}
	free(bf_filebuffer);
	return tape[head];
}

