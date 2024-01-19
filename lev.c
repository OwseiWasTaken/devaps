#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define GREEN "\x1b[1;32m"
#define RED "\x1b[1;31m"
#define RESET "\x1b[0m"

int trip(const int *mat, const int y, const int x, const int colcount) {
	const int a = mat[(y-1)*colcount+x];
	const int b = mat[y*colcount+(x-1)];
	const int c = mat[(y-1)*colcount+(x-1)];
	const int ab = (a<b)?a:b;
	return (ab<c)?ab:c;
}

void bmove(const int *mat, const int colcount, int *imin) {
	*imin = 0;
	for (int y = 0; y<colcount; y++) {
		if (mat[*imin] >= mat[y]) *imin=y;
	}
}

// actually implements wager-fisher edit distance but calling a tool "lev" is
// better than anything I came up with the words "wager fisher"
int main(int argc, char **argv) {
	int debug = 0;
	if (argc == 4) {
		debug = 1;
	} else {
		if (argc != 3) {
			fprintf(stderr, "Usage: %s [--debug] 'source' 'compare'\n", argv[0]);
			return 1;
		}
	}

	const char *w1 = argv[debug+1];
	const char *w2 = argv[debug+2];
	const int colcount = strlen(w1)+1;
	const int rowcount = strlen(w2)+1;
	int *mat = malloc(colcount*rowcount*sizeof(*mat));

	mat[0] = 0;

	for (int y = 0;y<rowcount;y++) {
		for (int x = 0;x<colcount; x++) {
			if (y && !x) {
				mat[y*colcount] = y;
			} else if (x && !y) {
				mat[x] = x;
			} else if (y && x) {;
				mat[y*colcount+x] = trip(mat, y, x, colcount)+(w2[y-1]!=w1[x-1]);
			}
		}
	}

	if (!debug) {
		printf("%d\n", mat[(rowcount-1)*colcount+colcount-1]);
		return 0;
	}

	printf("W-F");
	for (int x = 0;x<colcount;x++) {
		printf("[%c]", (!x)?'_':w1[x-1]);
	}
	printf("\n");
	for (int y = 0;y<rowcount;y++) {
		printf("[%c]", (!y)?'_':w2[y-1]);
		int index;
		bmove(&(mat[colcount*y]), colcount, &index);
		for (int x = 0;x<colcount; x++) {
			if (x == index) {
				printf("[%s%d%s]", GREEN, mat[y*colcount+x], RESET);
			} else {
				printf("[%d]", mat[y*colcount+x]);
			}
		}
		printf("\n");
	}
	printf("lev: %d\n", mat[(rowcount-1)*colcount+colcount-1]);
	return 0;
}

