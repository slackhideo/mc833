#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <string>

#define MAX_LINE 256

/* thread parameters structure */
typedef struct _params {
    int *next_msg_line;
    int socket;
    WINDOW *recvwin;
    WINDOW *sendwin;
    pthread_mutex_t mutex;
} params_t;

/* functions signatures */
void draw_borders(WINDOW *screen);
void *read_server(void *params);

/* main function */
int main(int argc, char *argv[]) {
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_in local;
    char *host;
    char buf[MAX_LINE];
    int s;
    unsigned int len;
    unsigned int locallen;

    char *name;
    int parent_x, parent_y, new_x, new_y;
    int msg_size = 4;
    int next_msg_line = 1;

    pthread_t thread;
    params_t params;

    if(argc != 4) {
        fprintf(stderr, "usage: %s <ip> <port> <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    host = argv[1];
    name = argv[3];

    /* translate host name into peer’s IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
        exit(EXIT_FAILURE);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(atoi(argv[2]));

    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(EXIT_FAILURE);
    }

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("simplex-talk: connect");
        close(s);
        exit(EXIT_FAILURE);
    }

    locallen = sizeof(local);
    if (getsockname(s, (struct sockaddr *)&local, &locallen) < 0) {
        perror("simplex-talk: getsockname");
        close(s);
        exit(EXIT_FAILURE);
    }

    /* send login information to server */
    strcpy(buf, name);
    buf[MAX_LINE-1] = '\0';
    len = strlen(buf) + 1;
    if(send(s, buf, len, 0) < 0) {
        perror("simplex-talk: send");
        exit(1);
    }

    /* ncurses initialisation */
    if(initscr() == NULL) {
        fprintf(stderr, "simplex-talk: ncurses\n");
        exit(EXIT_FAILURE);
    }
//    cbreak();

    keypad(stdscr, TRUE);
//    wtimeout(stdscr, 500);

    /* get maximum window dimensions */
    getmaxyx(stdscr, parent_y, parent_x);

    /* set up initial windows */
    WINDOW *recvwin = newwin(parent_y - msg_size, parent_x, 0, 0);
    WINDOW *sendwin = newwin(msg_size, parent_x, parent_y - msg_size, 0);
    leaveok(stdscr, TRUE);
    scrollok(recvwin, TRUE);

    /* draw to windows */
    draw_borders(recvwin);
    draw_borders(sendwin);

    /* thread initialisation */
    params.next_msg_line = &next_msg_line;
    params.socket = s;
    params.recvwin = recvwin;
    params.sendwin = sendwin;
    pthread_mutex_init(&params.mutex, NULL);
    pthread_create(&thread, NULL, read_server, &params);

    /* initial screen */
    pthread_mutex_lock(&params.mutex);
    mvwprintw(recvwin, next_msg_line++, 1, "Mensagens:");
    pthread_mutex_unlock(&params.mutex);
    mvwprintw(sendwin, 1, 1, "$[%s] ", name);
    wrefresh(recvwin);
    wrefresh(sendwin);

    /* main loop: get and send lines of text */
    while (wscanw(sendwin, "%s", buf)) {

        /* write user command in the screen */
        pthread_mutex_lock(&params.mutex);
        mvwprintw(recvwin, next_msg_line++, 1, "[%s] %s\n", name, buf);
        pthread_mutex_unlock(&params.mutex);

        /* send user command to server */
        buf[MAX_LINE-1] = '\0';
        len = strlen(buf) + 1;
        send(s, buf, len, 0);

        /* clear send window */
        wclear(sendwin);
        draw_borders(sendwin);
        mvwprintw(sendwin, 1, 1, "$[%s] ", name);

        wrefresh(recvwin);
        wrefresh(sendwin);

        /* check for window resize */
        getmaxyx(stdscr, new_y, new_x);
        if(new_y != parent_y || new_x != parent_x) {
            parent_x = new_x;
            parent_y = new_y;
            wresize(recvwin, new_y - msg_size, new_x);
            wresize(sendwin, msg_size, new_x);
            mvwin(sendwin, new_y - msg_size, 0);
            wclear(stdscr);
            wclear(recvwin);
            wclear(sendwin);
            draw_borders(recvwin);
            draw_borders(sendwin);
        }
    }

    pthread_join(thread, NULL);

    close(s);
    endwin();

    return EXIT_SUCCESS;
}

/* function to draw ncurses' windows borders */
void draw_borders(WINDOW *screen) {
    int x, y;
    int i;

    getmaxyx(screen, y, x);

    /* four borders */
    mvwprintw(screen, 0, 0, "+");
    mvwprintw(screen, y-1, 0, "+");
    mvwprintw(screen, 0, x-1, "+");
    mvwprintw(screen, y-1, x-1, "+");

    /* sides */
    for(i = 1; i < (y-1); i++) {
        mvwprintw(screen, i, 0, "|");
        mvwprintw(screen, i, x-1, "|");
    }

    /* top and bottom */
    for(i = 1; i < (x-1); i++) {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y-1, i, "-");
    }
}

void *read_server(void *params) {
    params_t *pars = (params_t *)params;
    char buf[MAX_LINE];

    /* wait for messages from server and display them */
    while(recv(pars->socket, buf, sizeof(buf), 0) > 0) {
        pthread_mutex_lock(&(pars->mutex));
// TODO: quebrar linhas e imprimí-las uma a uma
        mvwprintw(pars->recvwin, (*pars->next_msg_line)++, 1, "%s\n", buf);
        pthread_mutex_unlock(&(pars->mutex));
        wrefresh(pars->recvwin);
        wrefresh(pars->sendwin);
    }
    return NULL;
}
