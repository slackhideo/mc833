#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT 31472
#define MAX_LINE 256

int main(int argc, char *argv[]) {
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    int optval;

    if(argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(1);
    }
    host = argv[1];

    /* translate host name into peer's IP address */
    hp = gethostbyname(host);
    if(!hp) {
        fprintf(stderr, "client-udp: unknown host: %s\n", host);
        exit(1);
    }

    /* build address data structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* setup active open */
    if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("client-udp: socket");
        exit(1);
    }

    optval = 1;
    if(setsockopt(s, SOL_IP, IP_RECVERR, &optval, sizeof optval) < 0) {
        perror("client-udp: setsockopt");
        exit(1);
    }

    while(fgets(buf, sizeof(buf), stdin)) {
        buf[MAX_LINE-1] = '\0';
        if(sendto(s, buf, strlen(buf)+1, 0,
                    (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("client-udp: sendto");
        }
        if(recvfrom(s, buf, sizeof(buf), 0, NULL, NULL) > 0) {
            fprintf(stdout, "%s\n", buf);
        }
        else {
            perror("client-udp: recvfrom");
        }
    }

    close(s);

    return EXIT_SUCCESS;
}
