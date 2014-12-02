#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../liblog/liblog.h"

int main(int argc, char* argv[]) {
	int sleep_time, i = 0;
	char log_line[100];

    if (argc != 3) {
        printf("Usage: %s name pause\n", argv[0]);
        exit(1);
    }
    sleep_time = atoi(argv[2]);
	while (1) {
		sprintf(log_line, "this is a log line (%d)", i);
		++i;
		mylog(argv[1], log_line);
		if (sleep_time > 0)
			usleep(sleep_time);
	}
    return 0;
}
