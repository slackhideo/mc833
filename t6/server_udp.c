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
    char buf[MAX_LINE];
    int len;
    int s;

    /* build address data structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /* setup passive open */
    if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("server-udp: socket");
        exit(1);
    }

    if((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("server-udp: bind");
        exit(1);
    }

    for ever {
        while((len = recvfrom(s, buf, sizeof(buf), 0, NULL, NULL))) {
            if(len < 0) {
                perror("server-udp: recvfrom");
            }
            else {
                fputs(buf, stdout);
                if(sendto(s, buf, strlen(buf)+1, 0,
                                NULL, 0) < 0) {
                    perror("server-udp: sendto");
                }
            }
        }

        close(s);
    }

    return EXIT_SUCCESS;
}
