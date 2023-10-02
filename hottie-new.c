#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

// SIGUSR1 means child process finished

void sigdie(int signal) {
	exit(0);
}
const struct sigaction die = { .sa_handler = sigdie };

int master(pid_t child, int *filev, int filec) {
	const struct timespec await = {.tv_nsec = 10000000};

	printf("master of child %d\n", child);
	int wstatus = 0;
	while (1) {
		if (wstatus) {
			perror("wait status error");
		}
		waitpid(child, &wstatus, WNOHANG);
		printf("child has exited: %d\n", WIFEXITED(wstatus));
		printf("child exit code: %d\n", WEXITSTATUS(wstatus));
		//waitpid(child, &wstatus, 0);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &await, NULL);
		//if (WIFEXITED(wstatus)) {
		//	break;
		//}
	}
	printf("exited!\n");
	return 0;
}

void runner(const char *cmd) {
	//sigaction(SIGUSR2, &die, NULL);
	int ret = system(cmd);
	printf("c.sys: %d\n", ret);
	//execl("/bin/sh", "sh", "-c", cmd, (char *) NULL);
}

int main(const int argc, const char **argv) {
	if (argc < 3) {
		fprintf(stderr, "usage: %s command hot-file [hot-files]\n", argv[0]);
		return 1;
	}

	pid_t child;
	child = fork();
	assert(child >= 0);

	if (child != 0) {
		int filec = argc-2;
		int filev[filec];
		for (int i = 0; i<argc ; i++) {
			filev[i] = open(argv[i], 0);
		}
		return master(child, filev, argc-2);
	} else {
		runner(argv[1]);
	}
}
