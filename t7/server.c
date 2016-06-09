#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 31472
#define MAX_PENDING 5
#define MAX_LINE 256

#define ever (;;)

int main() {
    struct sockaddr_in sin;
    struct sockaddr_in peer;
    char buf[MAX_LINE];
    unsigned int ulen;
    int slen;
    unsigned int peerlen = sizeof(peer);
    int st, su, sconn;
    int pid;
    int nready;
    fd_set rset;

    /* build address data structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port        = htons(SERVER_PORT);

    /* setup passive open for TCP socket */
    if((st = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server: TCP socket");
        exit(1);
    }

    /* setup passive open for UDP socket */
    if((su = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("server: UDP socket");
        exit(1);
    }

    /* bind for TCP */
    if((bind(st, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("server: TCP bind");
        exit(1);
    }

    /* bind for UDP */
    if((bind(su, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("server: UDP bind");
        exit(1);
    }

    /* listen for TCP */
    if(listen(st, MAX_PENDING) < 0) {
        perror("server: listen");
        close(st);
        exit(1);
    }

    /* initialise the set of TCP and UDP sockets */
    FD_ZERO(&rset);
    FD_SET(st, &rset);
    FD_SET(su, &rset);

    for ever {

        /* select to get TCP or UDP incoming requests */
        if((nready = select(su+1, &rset, NULL, NULL, NULL)) < 0) {
            perror("server: select");
            close(st);
            close(su);
            exit(1);
        }

        /* TCP connection */
        if(FD_ISSET(st, &rset)) {
            ulen = sizeof(sin);

            if((sconn = accept(st, (struct sockaddr *)&sin, &ulen)) < 0) {
                perror("server: accept");
                close(st);
                close(su);
                exit(1);
            }

            /* fork a process for the child */
            if((pid = fork()) == 0) {
                close(st);

                while((slen = recv(sconn, buf, sizeof(buf), 0))) {
                    if(slen < 0) {
                        perror("server: recv");
                    }
                    else {
                        fprintf(stdout, "%s\n", buf);
                        if(send(sconn, buf, strlen(buf)+1, 0) < 0) {
                            perror("server: send");
                        }
                    }
                }

                close(sconn);
            }
            close(sconn);
        }

        /* UDP request */
        else if(FD_ISSET(su, &rset)) {
            while((slen = recvfrom(su, buf, sizeof(buf), 0,
                            (struct sockaddr *)&peer, &peerlen))) {
                if(slen < 0) {
                    perror("server: recvfrom");
                }
                else {
                    fprintf(stdout, "%s\n", buf);
                    if(sendto(su, buf, strlen(buf)+1, 0,
                                    (struct sockaddr *)&peer, peerlen) < 0) {
                        perror("server: sendto");
                    }
                }
            }

            close(su);
        }

        FD_SET(st, &rset);
        FD_SET(su, &rset);
    }

    return EXIT_SUCCESS;
}
