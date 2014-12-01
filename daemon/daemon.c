#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <time.h>

int main(int argc, char* argv[])
{
    FILE *fp= NULL;
    pid_t process_id = 0;
    pid_t sid = 0;

    process_id = fork();

    if (process_id < 0)
    {
        printf("fork failed!\n");

        exit(1);
    }

    if (process_id > 0)
    {
        printf("process_id of child process %d \n", process_id);

        exit(0);
    }

    umask(0);

    sid = setsid();
    if(sid < 0)
        {

        exit(1);
    }

    /* chdir("/"); */

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    fp = fopen ("Log.txt", "w+");
    while (1)
    {
        sleep(1);

        char buff[100];
        time_t now = time (0);
        strftime (buff, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
        fprintf(fp, "%s\n", buff);
        fflush(fp);
    }
    fclose(fp);
    return (0);
}
