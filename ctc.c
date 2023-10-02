#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int piped = 0;
void char_to_code(char *text) {
	for (int i = 0; text[i]; i++) {
		if (piped) {
			printf("%u\n", (unsigned int)text[i]);
		} else {
			printf("%c:%u\n", text[i], (unsigned int)text[i]);
		}
	}
}

void code_to_char(int code) {
	if (piped) {
		printf("%c", (char)code);
		return;
	}

	if (isprint(code)) {
		printf("%d:%c\n", code, (char)code);
	} else {
		printf("%d:0x%02x\n", code, code);
	}
}

int main(int argc, char **argv) {
	if (argc <= 1) return EXIT_SUCCESS;

	if (!strncmp(argv[1], "-1", 2)) {
		argc--; // skip flag
		argv = &(argv[1]);
		piped = 1;
	}

	// -1 flag to print
	for (int i = 1; i<argc; i++) {
		if (isdigit(argv[i][0])) {
			char *digit = strtok(argv[i], ":, \n");
			while (digit) {
				code_to_char(atoi(digit));
				digit = strtok(NULL, ":, \n");
			}
		} else {
			char_to_code(argv[i]);
		}
	}
	return EXIT_SUCCESS;
}

