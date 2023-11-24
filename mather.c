#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define _SIMPLE_VECTOR_IMPLEMENTATION
#include "simple.h"

int c_in_str(char needle, char *haystack) {
	do {
		if (*haystack == needle) return 1;
	} while (*(++haystack));
	return 0;
}

typedef enum  {
	OP_UNKNOWN = -1,
	OP_ADD = 1,
	OP_SUB = 2,
	OP_DIV = 3,
	OP_MUL = 4,
	OP_CALL = 5,
} OP_TYPE;

char *op_to(OP_TYPE op) {
	switch (op) {
	case OP_UNKNOWN: return "UNKNOWN";
	case OP_ADD: return "ADD";
	case OP_SUB: return "SUB";
	case OP_DIV: return "DIV";
	case OP_MUL: return "MUL";
	case OP_CALL: return "CALL";
	}
	return "UNDEFINED OPERATION";
}

typedef enum  {
	FUNC_UNKNOWN = -1,
	FUNC_SQRT = 1,
} FUNC_TYPE;

// even (and equal) IDs can concat
typedef enum {
	TOK_UNKNOWN = -1,
	TOK_PAREM_O = 1,
	TOK_NUMBER = 2, // can cat
	TOK_PAREM_C = 3,
	TOK_STRING = 4, // can cat
	TOK_OP = 5,
	TOK_IGNORE = 6, // can cat
} TOK_TYPE;

char *tok_to(TOK_TYPE tok) {
	switch (tok) {
	case TOK_UNKNOWN: return "UNKNOWN";
	case TOK_PAREM_O: return "PAREM_O";
	case TOK_NUMBER: return "NUMBER";
	case TOK_PAREM_C: return "PAREM_C";
	case TOK_STRING: return "STRING";
	case TOK_IGNORE: return "<IGNORE>";
	case TOK_OP: return "OP";
	}
	return "UNDEFINED TOKEN";
}

TOK_TYPE to_tok(char tok) {
	if (c_in_str(tok, "0123456789"))
		return TOK_NUMBER;
	if (c_in_str(tok, "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_"))
		return TOK_STRING;
	if (c_in_str(tok, "+-/*!"))
		return TOK_OP;

	switch (tok) {
		case '(': return TOK_PAREM_O;
		case ')': return TOK_PAREM_C;
		case ' ': return TOK_IGNORE;
	}
	return TOK_UNKNOWN;
}

OP_TYPE to_op(char opchar) {
	switch (opchar) {
	case '+': return OP_ADD;
	case '-': return OP_SUB;
	case '/': return OP_DIV;
	case '*': return OP_MUL;
	case '!': return OP_CALL;
	}
	return OP_UNKNOWN;
}

typedef struct {
	char *text;
	int off;
	int range; // expr[off:off+range]
	TOK_TYPE type;
} token_point;

typedef struct _eval {
	OP_TYPE op_type;
	char *text; // strncpy of expr
	struct _eval *left_eval;
	struct _eval *right_eval;
	long double *value; // NULL if not yet defined
} eval_point;

int main(int argc, char **argv) {
	assert(argc > 1);

	int expr_size = 0;

	for (int i = 1; i<argc; i++) {
		printf("[%d] \"%s\"\n", i, argv[i]);
		expr_size += 1+strlen(argv[i]);
	}

	char *expr = malloc(expr_size);

	for (int off = 0, i = 1; i<argc; i++) {
		if (strlen(argv[i])) {
			strcat(expr+off, argv[i]);
			strcat(expr+off+1, " ");
			off+=1+strlen(argv[i]);
		}
	}

	vector *token_vector = vec_make(0, 0, NULL);
	token_point *last_tok = NULL;
	for (int i = 0; i<expr_size; i++) {
		TOK_TYPE tok_num = to_tok(expr[i]);

		if (tok_num % 2 == 0) { // cat-able token
			// there's a token in cat buffer
			if (last_tok != NULL) {
				if (last_tok->type == tok_num) {
					// append to last token
					last_tok->range++;
				} else {
					// commit last token
					last_tok->text = strndup(expr+last_tok->off, last_tok->range);
					if (last_tok->type != TOK_IGNORE) {
						vec_push(token_vector, last_tok);
					} else {
						free(last_tok);
					}
					// set to create new token
					last_tok = NULL;
				}
			}

			if (last_tok == NULL) {
				last_tok = malloc(sizeof(token_point));
				last_tok->off = i;
				last_tok->range = 1;
				last_tok->type = tok_num;
			}
		} else { // non- cat-able token
			if (last_tok != NULL) {
				// commit last token
				last_tok->text = strndup(expr+last_tok->off, last_tok->range);
				if (last_tok->type != TOK_IGNORE) {
					vec_push(token_vector, last_tok);
				} else {
					free(last_tok);
				}
				last_tok = NULL;
			}
			// make and commit token
			last_tok = malloc(sizeof(token_point));
			last_tok->text = strndup(expr+i, 1);
			last_tok->off = i;
			last_tok->range = 1;
			last_tok->type = tok_num;
			if (last_tok->type != TOK_IGNORE) {
				vec_push(token_vector, last_tok);
			} else {
				free(last_tok);
			}
			last_tok = NULL;
		}
	}
	if (last_tok != NULL) {
		last_tok->text = strndup(expr+last_tok->off, last_tok->range);
		vec_push(token_vector, last_tok);
		last_tok = NULL;
	}

	for (int i = 0; i<vec_size(token_vector); i++) {
		last_tok = vec_get(token_vector, i);
		printf("%d[%d:%d] \"%s\" {%s}\n", i, last_tok->off,
				last_tok->off+last_tok->range, last_tok->text, tok_to(last_tok->type));
	}

	free(expr);
}

