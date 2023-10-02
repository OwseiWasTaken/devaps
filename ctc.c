#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void char_to_code(char *text) {
	for (int i = 0; i<strlen(text); i++) {
		printf("%c:%d\n", text[i], (int)text[i]);
	}
}

void code_to_char(int code) {
	if (isprint(code)) {
		printf("%d:%c\n", code, (char)code);
	} else {
		printf("%d:\\x%02x\n", code, code);
	}
}

int main(int argc, char **argv) {
	// -1 flag to print
	for (int i = 1; i<argc; i++) {
		if (isdigit(argv[i][0])) {
			// split with strtok(":-, ")
			code_to_char(atoi(argv[i]));
		} else {
			char_to_code(argv[i]);
		}
	}
}

