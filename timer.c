

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
	char* label;
	float total;
} timer;

int main(int argc, char *argv[]) {
	timer timers[argc];
	struct timespec span = {0};
	long double tspan = 0;

	struct timespec start = {0};
	struct timespec now = {0};
	// 0.1s
	struct timespec step = { .tv_nsec = 100000000 };
	clock_gettime(CLOCK_MONOTONIC, &start);
	//start.tv_nsec = 0;

	for (int i = 1; i<argc; i++) {
		char *time = strtok(argv[i], ":");
		char *label = strtok(NULL, ":");

		timer a = { .label = label?label:"timer", .total = atof(time) };
		timers[i] = a;
	}

	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		span.tv_sec = now.tv_sec - start.tv_sec;
		span.tv_nsec = (now.tv_nsec - start.tv_nsec)/100000;
		tspan = (long double)span.tv_sec + ((long double)span.tv_nsec)/10000;
		//printf("%Lf:%Lf\n", (long double)span.tv_sec, ((long double)span.tv_nsec)/1000);

		for (int i = 1; i<argc; i++) {
			printf("%d: %s: %f\n", i, timers[i].label, timers[i].total);
		}

		printf("span: %Lf\n", tspan);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &step, NULL);
	}
	return 0;
}

