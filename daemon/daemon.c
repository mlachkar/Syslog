#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>

#include <time.h>

#define MAXTHREADS 10

FILE *fp= NULL;

char mesg[MAXTHREADS][1000];

pthread_mutex_t lock;
sem_t message_waiting;
int in = 0;
int out = 0;

struct timespec t;
struct tm* tt;
float seconds;
time_t now;


void* thread_routine (void* arg) {

    char time_buff[100];
    char name[100];
    char message[1000];
    char* limit;

    while (1) {
        sem_wait(&message_waiting);

        limit = index(mesg[out], ':');
        strncpy(name, mesg[out], limit - mesg[out]);
        name[limit - mesg[out]] = 0;
        strcpy(message, limit + 1);

        pthread_mutex_lock(&lock);

        now = time (0);
        clock_gettime( CLOCK_REALTIME, &t );
        tt = gmtime( &(t.tv_sec) );
        seconds = (float)tt->tm_sec + ((float)t.tv_nsec / 1000000000.0);

        strftime (time_buff, 100, "%Y-%m-%d %H:%M", localtime (&now));

        fprintf(fp, "%s:%02.3f %s %s\n", time_buff,seconds, name, message);

        fflush(fp);
        out = (out+1)%MAXTHREADS;

        pthread_mutex_unlock(&lock);
    }
}

int main(int argc, char* argv[]) {

    pthread_t thread_id[MAXTHREADS];

    pid_t process_id = 0;
    pid_t sid = 0;
    int i;



    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char raw_mesg[1000];

    if (argc != 2) {
        printf("Usage: %s log_file\n", argv[0]);
        exit(1);
    }

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

    fp = fopen (argv[1], "w+");


    pthread_mutex_init(&lock, NULL);
    sem_init(&message_waiting, 0, 0);

    for (i=0; i < MAXTHREADS;i++) {
        assert((pthread_create(&(thread_id[i]),NULL,
        thread_routine,NULL)) == 0);
        printf("created thread %lu\n",(thread_id[i]));
        fflush(stdout);
    }

    printf("plop\n");

    // for (i=0; i < MAXTHREADS;i++) {
    //     printf("Mama waiting for thread %lu to end\n", thread_id[i]);
    //     fflush(stdout);
    //     assert((pthread_join(thread_id[i], NULL)) == 0);
    // }

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


    printf("plop\n");

    while (1)
        {
            len = sizeof(cliaddr);
            n = recvfrom(sockfd,mesg[in],1000,0,(struct sockaddr *)&cliaddr,&len);
            sendto(sockfd,raw_mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
            mesg[in][n] = 0;
            printf("plop\n");

            in = (in+1)%MAXTHREADS;
            sem_post(&message_waiting);

        }


    pthread_mutex_destroy(&lock);
    sem_destroy(&message_waiting);


    fclose(fp);
    return (0);
}
