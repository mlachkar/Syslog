#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../liblog/liblog.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage: %s name\n", argv[0]);
        exit(1);
    }

    mylog(argv[1], "A different kind of Hello world ... ");

    return 0;
}
