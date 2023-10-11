#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

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

char *stringify_tape(char *tape, int size) {
	char *text = malloc(size*5);
	for (int i = 0; i<size; i++) {
		snprintf(text+(i*5), 6, "%03hhu, ", tape[i]);
	}
	text[size*5-2] = ' ';
	text[size*5-1] = '\0';
	return text;
}

int main(const int argc, const char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <file.bf>\n", argv[0]);
		return -1;
	}

	const char* bf_filename = argv[1];
	struct stat statbuf;
	assert(stat(bf_filename, &statbuf) == 0);
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
	char bf_text_buffer[bf_size+1];
	bf_text_buffer[bf_size] = 0;
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
	char tape[TAPE_SIZE] = {0};
	size_t head = 0;
	size_t headmax = 0;

	initscr();
	WINDOW *winbf_src    = newwin(12, COLS, 0, 0);
	WINDOW *winbf_status = newwin(4, COLS, 12, 0);
	WINDOW *winbf_stdout = newwin(5, COLS, 12+4, 0);
	WINDOW *winbf_tape   = newwin(LINES-(12+4+5), COLS, 12+4+5, 0);

	//box(winbf_status, -1, -1);
	//box(winbf_src, -1, -1);
	//box(winbf_tape, -1, -1);

	cbreak();
	noecho();
	curs_set(0);

	mvwhline(winbf_status, 0, 0, 0, COLS);
	mvwprintw(winbf_status, 0, 2, "< %s >", argv[1]);

	mvwhline(winbf_status, 3, 0, 0, COLS);
	mvwprintw(winbf_status, 3, 2, "< output >");

	mvwhline(winbf_stdout, 4, 0, 0, COLS);
	mvwprintw(winbf_stdout, 4, 2, "< memory tape >");
	wmove(winbf_stdout, 0, 0);

	wrefresh(winbf_status);
	wrefresh(winbf_stdout);
	int ch = ' ';

	for (size_t clock = 0, i = 0; i<bf_size; i++, clock++) {
		mvwprintw(winbf_src, 0, 0, "%s", bf_text_buffer);
		mvwaddch(winbf_src, i/COLS, i%COLS, bf_text_buffer[i] | A_REVERSE);
		wrefresh(winbf_src);

		mvwprintw(winbf_status, 1, 0, "tick: %ld; furthest tape block: %ld  ", clock, headmax);
		mvwprintw(winbf_status, 2, 0, "head value: %hhu; tape head: %ld k:%c k:%d  ", tape[head], head, ch, ch);
		//mvwprintw(winbf_status, 2, 0, "char: %ld,%ld  ", i%COLS, i/COLS);
		wrefresh(winbf_status);

		//TODO do huge allocs lates
		if (headmax < 100 ) {
			char *tape_view = stringify_tape(tape, headmax+1);
			mvwprintw(winbf_tape, 0, 0, "%s", tape_view);
			free(tape_view);
			mvwaddch(winbf_tape, head*5/COLS, head*5%COLS+3, ' ' | A_REVERSE);
		} else { // or get centering program
			mvwprintw(winbf_tape, 0, 0, "tape too big to display");
			//mvwprintw(winbf_tape, 0, 0, "tape not displayer, -O3 detected");
		}
		wrefresh(winbf_tape);

		//ch = wgetch(winbf_src);

		cop = bf_buffer[i];
		switch(cop.type) {
		case StartLoop:
			if (!tape[head]) {
				i = cop.index-1;
			}
			break;
		case EndLoop:
			if (tape[head]) {
				i = cop.index;
			}
			break;
		case HeadLeft:
			//TODO: should declare tape limit
			if (head == 0) head = headmax+1;
			//assert(head > 0);
			head--;
			break;
		case HeadRight:
			assert(head < TAPE_SIZE);
			if (headmax == head) headmax++;
			head++;
			break;
		case Increment:
			tape[head]++;
			break;
		case Decrement:
			tape[head]--;
			break;
		case GetKey:
			wmove(winbf_status, 1, 0);
			wclrtoeol(winbf_status);
			wmove(winbf_status, 2, 0);
			wclrtoeol(winbf_status);

			mvwprintw(winbf_status, 1, 0, "%s if asking for input!", argv[1]);
			mvwprintw(winbf_status, 2, 0, "press enter");
			while (wgetch(winbf_status) != '\n') {}
			mvwprintw(winbf_status, 2, 0, "press your input key");
			ch = wgetch(winbf_status);
			tape[head] = (char)ch;
			break;
		case PrintKey:
			if (tape[head] == '\n') {
				waddch(winbf_stdout, ',');
				waddch(winbf_stdout, ' ');
				wrefresh(winbf_stdout);
			} else {
				waddch(winbf_stdout, tape[head]);
				wrefresh(winbf_stdout);
			}
			//fputc(tape[head], stdout);
			break;
		}
	}
	endwin();
	free(bf_filebuffer);
	return tape[head];
}

