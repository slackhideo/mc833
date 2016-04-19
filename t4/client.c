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
#define MAX_LINE 256

int main(int argc, char * argv[])
{
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_in local;
    char *host;
    char buf[MAX_LINE];
    int s;
    unsigned int len;
    unsigned int locallen;

    if (argc==2) {
        host = argv[1];
    }
    else {
        fprintf(stderr, "usage: ./client host\n");
        exit(1);
    }

    /* translate host name into peer’s IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
        exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("simplex-talk: connect");
        close(s);
        exit(1);
    }

    locallen = sizeof(local);
    if (getsockname(s, (struct sockaddr *)&local, &locallen) < 0) {
        perror("simplex-talk: getsockname");
        close(s);
        exit(1);
    }

    fprintf(stdout, "-------------------\n");
    fprintf(stdout, "IP local: %s\n", inet_ntoa(local.sin_addr));
    fprintf(stdout, "Porta local: %d\n", ntohs(local.sin_port));
    fprintf(stdout, "-------------------\n\n");

    /* main loop: get and send lines of text */
    while (fgets(buf, sizeof(buf), stdin)) {
        buf[MAX_LINE-1] = '\0';
        len = strlen(buf) + 1;
        send(s, buf, len, 0);
        if (recv(s, buf, sizeof(buf), 0) > 0) {
            fputs(buf, stdout);
            fputs("\n", stdout);
        }
    }
    close(s);

    return EXIT_SUCCESS;
}
