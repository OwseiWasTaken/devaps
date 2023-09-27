#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int stats(const char *filev[], int filec, long int lastmods[]) {
	struct stat statbuf;
	int changed = 0;
	for (int i = 0; i < filec; i++) {
		stat(filev[i], &statbuf);
		if (lastmods[i] != statbuf.st_mtim.tv_sec) {
			lastmods[i] = statbuf.st_mtim.tv_sec;
			changed = 1;
		}
	}
	return changed;
}

int main (const int argc, const char *argv[]) {

	// TODO: open(argv[1]); fstat(fd)
	if (argc < 3) {
		fprintf(stderr, "usage: %s <command> file[s]\n\
if the modtime of any file changes, <command> is executed in sh\n", argv[0]);
		return 1;
	}
	assert(argc >= 3); // usage ./file <hot file>

	const struct timespec await = {.tv_nsec = 10000000};
	// 1_000_000_000ns = 1s

	pid_t child = 0;

	const int filec = argc-2;
	const char **filev = &(argv[2]);
	const char *cmd = argv[1];
	long int lastmods[filec];

	while (1) {
		clock_nanosleep(CLOCK_MONOTONIC, 0, &await, NULL);
		// file mod guard
		if (!stats(filev, filec, lastmods)) continue;

		if (child) kill(child, SIGTERM);

		child = fork();
		assert(child >= 0);

		// is child guard
		if (child) continue;

		// is child
		execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
		pid_t parent = getppid();
		if (parent != 1) { // not init(1)
			fprintf(stderr, "failed to execute `sh`, with command %s\n", cmd);
			kill(getppid(), SIGTERM);
		}
		return 1;

	}
	return 0;
}

