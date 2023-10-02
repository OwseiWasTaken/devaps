

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	char* label;
	float total;
} timer;

int main(int argc, char *argv[]) {
	timer timers[argc];
	struct timespec span = {0};

	struct timespec start = {0};
	struct timespec now = {0};
	// 0.1s
	struct timespec step = { .tv_nsec = 100000000 };
	clock_gettime(CLOCK_MONOTONIC, &start);
	//start.tv_nsec = 0;

	for (int i = 1; i<argc; i++) {
		// strtok(":") twice to get number[:label]
		timer a = { .label = "uwu", .total = atof(argv[i]) };
		timers[i] = a;
	}

	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		span.tv_sec = now.tv_sec - start.tv_sec;
		span.tv_nsec = (now.tv_nsec - start.tv_nsec)/100000;

		for (int i = 1; i<argc; i++) {
			printf("%d: %s: %f\n", i, timers[i].label, timers[i].total);
		}
		//printf("span: %ld.%.4ld\n", span.tv_sec, span.tv_nsec);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &step, NULL);
	}
	return 0;
}

