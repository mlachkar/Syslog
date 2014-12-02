#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../liblog/liblog.h"

int main(int argc, char* argv[]) {
	int i;
    if (argc != 3) {
        printf("Usage: %s name & integer \n", argv[0]);
        exit(1);
    }
    int imax = atoi(argv[2]);
	for (i = 0 ; i < imax ; i++)
	{
		mylog(argv[1], "A different kind of Hello world ... ");
	}
    return 0;
}
