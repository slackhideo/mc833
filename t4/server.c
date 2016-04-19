#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT 31472
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
    struct sockaddr_in sin;
    struct sockaddr_in peer;
    char buf[MAX_LINE];
    unsigned int len;
    unsigned int peerlen;
    int s, new_s;

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    }

    listen(s, MAX_PENDING);

    /* wait for connection, then receive and print text */
    while(1) {
        len = sizeof(sin);
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            perror("simplex-talk: accept");
            exit(1);
        }

        peerlen = sizeof(peer);
        if (getpeername(new_s, (struct sockaddr *)&peer, &peerlen) < 0) {
            perror("simplex-talk: getpeername");
            exit(1);
        }

        fprintf(stdout, "--------------------\n");
        fprintf(stdout, "IP remoto: %s\n", inet_ntoa(peer.sin_addr));
        fprintf(stdout, "Porta remota: %d\n", ntohs(peer.sin_port));
        fprintf(stdout, "--------------------\n\n");

        while ((len = recv(new_s, buf, sizeof(buf), 0))) {
            fputs(buf, stdout);
            if ((send(new_s, buf, strlen(buf)+1, 0)) < 0) {
                perror("simplex-talk: send");
                exit(1);
            }
        }
        close(new_s);
    }

    return EXIT_SUCCESS;
}
