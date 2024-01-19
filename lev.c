#include <stdio.h>
#include <malloc.h>
#include <string.h>

int trip(const int *mat, const int y, const int x, const int colcount) {
	const int /*top*/a = mat[(y-1)*colcount+x]; // delete
	const int /*left*/b = mat[y*colcount+(x-1)]; // insert
	const int /*top left*/c = mat[(y-1)*colcount+(x-1)]; // sub
	const int ab = (a<b)?a:b;
	return (ab<c)?ab:c;
}

int wagfish(
	const int colcount, // strlen(w1)
	const int rowcount, // strlen(w2)
	const int matcolcount, // col count of matrix
	const int matrowcount, // row count of matrix
	int mat[matrowcount*matcolcount], // matrix
	const char *w1, const char *w2 // w1, w2
) {
	mat[0] = 0;

	for (int y = 0;y<rowcount;y++) {
		for (int x = 0;x<colcount; x++) {
			if (y && !x) {
				mat[y*matcolcount] = y;
			} else if (x && !y) {
				mat[x] = x;
			} else if (y && x) {;
				mat[y*matcolcount+x] = trip(mat, y, x, colcount)+(w2[y-1]!=w1[x-1]);
			}
		}
	}
	return mat[(rowcount-1)*matcolcount+colcount-1];
}

// actually implements Wagner–Fischer edit distance but calling a tool "lev" is
// better than anything I can come up with using the words "Wagner" & "Fischer"
int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s 'source' ['compare' ...]\n", argv[0]);
		return 1;
	}

	const char *target = argv[1];
	const int colcount = strlen(target)+1;
	const int matcolcount = colcount;

	int lens[argc-2];
	int matrowcount = 0;

	for (int i = 2; i < argc; i++) {
		lens[i] = strlen(argv[i])+1;
		if (lens[i] > matrowcount) {
			matrowcount=lens[i];
		}
	}

	int *mat = malloc(matcolcount*matrowcount*sizeof(*mat));

	for (int i = 2; i < argc; i++) {
		const char *w2 = argv[i];
		const int rowcount = lens[i];
		int lev = wagfish(colcount, rowcount, matcolcount, matrowcount, mat, target, w2);
		printf("%d %s\n", lev, w2);
	}

	free(mat);
	return 0;
}

