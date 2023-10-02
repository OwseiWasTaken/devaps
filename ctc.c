#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char* code_to_escape[6] = {
	"\\b", "\\t", "\\n",
	"\\v", "\\f", "\\r",
};

int piped = 0;
void char_to_code(char *text) {
	for (int i = 0; text[i]; i++) {
		if (piped) {
			printf("%u\n", (unsigned int)text[i]);
		} else {
			if (8 <= text[i] && text[i] <=13) {
				printf("%s:%u\n", code_to_escape[text[i]-8], (unsigned int)text[i]);
			} else {
				printf("%c:%u\n", text[i], (unsigned int)text[i]);
			}
		}
	}
}

// segfaults when code is too big (like 123456789)
void code_to_char(unsigned int code) {
	if (piped) {
		printf("%c", (unsigned char)code);
		return;
	}

	if (8 <= code && code <=13) {
		printf("%d:%s\n", code, code_to_escape[code-8]);
	} else if ( isprint(code) ) {
		printf("%d:%c\n", code, (unsigned char)code);
	} else {
		printf("%d:0x%02x\n", code, code);
	}
}

void print_line(char *line) {
	if (isdigit(line[0])) {
		char *digit = strtok(line, ":, \n");
		while (digit) {
			code_to_char(atoi(digit));
			digit = strtok(NULL, ":, \n");
		}
	} else {
		char_to_code(line);
	}
}

// TODO: implement reading from stdin
int main(int argc, char **argv) {
	if (argc > 1) {
		if (!strncmp(argv[1], "-1", 2)) {
			argc--; // skip flag
			argv = &(argv[1]);
			piped = 1;
		}
	}

	if (argc == 1) {
		char buffer[BUFSIZ];
		int read = scanf("%s\n", buffer);
		while (read > 0) {
			print_line(buffer);
			read = scanf("%s\n", buffer);
		}
		return EXIT_SUCCESS;
	}

	for (int i = 1; i<argc; i++) {
		print_line(argv[i]);
	}
	return EXIT_SUCCESS;
}

