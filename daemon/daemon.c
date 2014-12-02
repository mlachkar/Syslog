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

/* log file */
FILE *fp= NULL;

/* shared log entries waiting to be written */
char mesg[MAXTHREADS][1000];

/* threading */
pthread_mutex_t lock;
sem_t message_waiting;
sem_t logger_waiting;
int in = 0;
int out = 0;

/* time management */
struct timespec t;
struct tm* tt;
float seconds;
time_t now;

/* log line management */
char time_buff[100];
char name[100];
char message[1000];
char* limit;

void* thread_routine (void* arg) {
    while (1) {
        assert(sem_wait(&message_waiting) == 0);
        assert(pthread_mutex_lock(&lock) == 0);

        /* parse log line */
        limit = index(mesg[out], ':');
        strncpy(name, mesg[out], limit - mesg[out]);
        name[limit - mesg[out]] = 0;
        strcpy(message, limit + 1);

        /* compute time */
        now = time (0);
        clock_gettime( CLOCK_REALTIME, &t );
        tt = gmtime( &(t.tv_sec) );
        seconds = (float)tt->tm_sec + ((float)t.tv_nsec / 1000000000.0);

        /* format log line */
        strftime(time_buff, 100, "%Y-%m-%d %H:%M", localtime (&now));

        /* write to the logfile */
        assert(fprintf(fp, "%s:%02.3f %s %s\n", time_buff,seconds, name, message) > -1);
        assert(fflush(fp) == 0);

        /* Increment the id of the next line to be treated */
        out = (out+1)%MAXTHREADS;

        assert(pthread_mutex_unlock(&lock) == 0);
        assert(sem_post(&logger_waiting) == 0);
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

    /* receiving buffer */
    char raw_mesg[1000];

    if (argc != 2) {
        printf("Usage: %s log_file\n", argv[0]);
        exit(1);
    }

    /* daemonizing */
    process_id = fork();

    if (process_id < 0) {
        printf("fork failed!\n");

        exit(1);
    }

    if (process_id > 0) {
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

    fp = fopen(argv[1], "w+");

    assert(pthread_mutex_init(&lock, NULL) == 0);
    assert(sem_init(&message_waiting, 0, 0) == 0);
    assert(sem_init(&logger_waiting, 0, MAXTHREADS) == 0);

    /* create threads */
    for (i=0; i < MAXTHREADS;i++) {
        assert((pthread_create(&(thread_id[i]),NULL,
        thread_routine,NULL)) == 0);
        printf("created thread %lu\n",(thread_id[i]));
        fflush(stdout);
    }

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    assert(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == 0);

    while (1) {
        assert(sem_wait(&logger_waiting) == 0);
        assert(pthread_mutex_lock(&lock) == 0);
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg[in],1000,0,(struct sockaddr *)&cliaddr,&len);
        assert(sendto(sockfd,raw_mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr)) > -1);
        mesg[in][n] = 0;

        in = (in+1)%MAXTHREADS;
        assert(pthread_mutex_unlock(&lock) == 0);
        assert(sem_post(&message_waiting) == 0);
    }

    assert(pthread_mutex_destroy(&lock) == 0);
    assert(sem_destroy(&message_waiting) == 0);
    assert(sem_destroy(&logger_waiting) == 0);

    assert(fclose(fp) == 0);
    return (0);
}
