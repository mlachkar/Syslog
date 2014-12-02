#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

void mylog(char* name, char* message) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[1000];

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    servaddr.sin_port=htons(32000);

    sprintf(sendline, "%s[%d]:%s", name, getpid(), message);

    /* send the log line */
    assert(sendto(sockfd,sendline,strlen(sendline),0,
    (struct sockaddr *)&servaddr,sizeof(servaddr)) > -1);
}
