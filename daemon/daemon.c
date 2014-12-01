#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <time.h>

int main(int argc, char* argv[])
{
    /* Log file */
    FILE *fp= NULL;

    pid_t process_id = 0;
    pid_t sid = 0;

    time_t now;

    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char raw_mesg[1000];

    char time_buff[100];
    char name[100];
    char message[1000];
    char* limit;

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
    if(sid < 0) {
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    fp = fopen ("mylog.txt", "w+");
    
    while (1)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,raw_mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        sendto(sockfd,raw_mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        raw_mesg[n] = 0;

        limit = index(raw_mesg, ':');
        strncpy(name, raw_mesg, limit - raw_mesg);
        name[limit - raw_mesg] = 0;
        strcpy(message, limit + 1);

        now = time (0);
        strftime (time_buff, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
        fprintf(fp, "%s %s %s\n", time_buff, name, message);
        fflush(fp);
    }
    fclose(fp);
    return (0);
}
