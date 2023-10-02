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

void code_to_char(int code) {
	if (piped) {
		printf("%c", (char)code);
		return;
	}

	if ( isprint(code) || (8 <= code && code <=13) ) {
		if (8 <= code && code <=13) {
			printf("%d:%s\n", code, code_to_escape[code-8]);
		} else {
			printf("%d:%c\n", code, (char)code);
		}
	} else {
		printf("%d:0x%02x\n", code, code);
	}
}

// TODO: implement reading from stdin
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

