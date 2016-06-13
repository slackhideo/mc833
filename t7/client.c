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
    char *mode, *host;
    char buf[MAX_LINE];
    int s;
    int tcp = -1;

    if(argc != 3) {
        fprintf(stderr, "usage: %s <mode> <host>\n \
mode:\n-t for TCP\n-u for UDP\n", argv[0]);
        exit(1);
    }
    mode = argv[1];
    host = argv[2];

    if(strcmp(mode, "-t") == 0) {
        tcp = 1;
    }
    else if(strcmp(mode, "-u") == 0) {
        tcp = 0;
    }
    else {
        fprintf(stderr, "unknown mode\nvalid modes are -t and -u\n");
        exit(1);
    }

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

    /* TCP mode */
    /* setup active open */
    if(tcp) {
        if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            perror("client: TCP socket");
            exit(1);
        }
    }

    /* UDP mode */
    /* setup active open */
    else {
        if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("client: UDP socket");
            exit(1);
        }
    }

    if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("client: connect");
        close(s);
        exit(1);
    }

    /* main loop: get and send text */
    while(fgets(buf, sizeof(buf), stdin)) {
        buf[MAX_LINE-1] = '\0';
        if(send(s, buf, strlen(buf)+1, 0) < 0) {
            perror("client: send");
        }
        if(recv(s, buf, sizeof(buf), 0) > 0) {
            fprintf(stdout, "%s\n", buf);
        }
        else {
            perror("client: recv");
        }
    }

    close(s);

    return EXIT_SUCCESS;
}
