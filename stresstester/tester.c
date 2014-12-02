#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../liblog/liblog.h"

int main(int argc, char* argv[]) {
	int i, imax;
	char log_line[100];

    if (argc != 3) {
        printf("Usage: %s name number\n", argv[0]);
        exit(1);
    }
    imax = atoi(argv[2]);
	for (i = 0 ; i < imax ; i++)
	{
		sprintf(log_line, "this is a log line (%d)", i);
		mylog(argv[1], log_line);
	}
    return 0;
}
