#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

void sigignore(int signal) {}

pid_t child = 0;
int *fdv;
int filec;
long int *lastmods;
void kill_child() {
	char *killchildren = malloc(1024);
	snprintf(killchildren, 1024,
"children=$(pstree -p %d |sed \"s|-+\\?-\\?-|\\n|g\" | sed 's|[a-zAZ{} -|`]*(\\([0-9]\\+\\))|\\1|')\n\
for i in $children\n\
do \n kill $i \n done" , child);
	assert(!system(killchildren));
	free(killchildren);

	kill(child, SIGTERM);
	waitpid(child, NULL, 0); // must wait for child death
}

void sighandle(int signal) {
	if (signal != SIGINT)
		fprintf(stderr, "Killed by signal %d\n", signal);
	if (child) kill_child();
	if (filec && fdv) {
		for (int i = 0; i < filec; i++) {
			close(fdv[i]);
		}
		free(fdv);
	}
	if (lastmods) free(lastmods);
	exit(0);
}

int stats(const int filev[], int filec, long int lastmods[]) {
	struct stat statbuf;
	int changed = 0;
	for (int i = 0; i < filec; i++) {
		fstat(filev[i], &statbuf);
		if (lastmods[i] != statbuf.st_mtim.tv_sec) {
			lastmods[i] = statbuf.st_mtim.tv_sec;
			changed = 1;
		}
	}
	return changed;
}

// split into 2 processes, master, child runner & child
int main (const int argc, const char **argv) {
	//TODO: open(argv[1]); fstat(fd)

	if (argc < 3 || (argc == 2 && !strncmp(argv[1], "--help", 6))) {
		fprintf(stderr,
"usage: %s [--hup] <command> file[s]\n\
	if the modtime of any file changes, <command> is executed in sh\n\
	if 'hup' flag is defined, child process will also be killed when handeling 1) SIGHUP signal\n\
", argv[0]);
		return 1;
	}
	assert(argc >= 3); // usage ./hottie [--hup] "cmd" <hot files>

	const struct timespec await = {.tv_nsec = 10000000};
	// 1_000_000_000ns = 1s

	int onhang = 0;
	if (!strncmp(argv[1], "--hup", 5)) {
		onhang = 1;
		// (not really removing --hup, but has same effect)
	}

	struct sigaction act = { .sa_handler = sighandle };
	struct sigaction ignore = { .sa_handler = sigignore };
	if (onhang) sigaction(SIGHUP, &act, NULL);
	else sigaction(SIGHUP, &ignore, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGPWR, &act, NULL);

	const char *cmd = argv[1+onhang];
	const char **filev = &(argv[2+onhang]);
	filec = argc-2-onhang;
	fdv = malloc(sizeof(int)*filec);
	for (int i = 0; i < filec; i++) {
		fdv[i] = open(filev[i], O_RDONLY|O_WRONLY); // cant read nor write;
	}
	lastmods = calloc(filec, sizeof(long int));

	while (1) {
		clock_nanosleep(CLOCK_MONOTONIC, 0, &await, NULL);
		// file mod guard
		if (!stats(fdv, filec, lastmods)) continue;

		if (child) kill_child();

		child = fork();
		assert(child >= 0);

		// is child guard
		if (child) continue; // await termination

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

