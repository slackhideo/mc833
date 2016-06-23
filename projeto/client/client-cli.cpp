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
#include <string>

#define MAX_LINE 256

void draw_borders(WINDOW *screen);

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

    /* draw to windows */
    draw_borders(recvwin);
    draw_borders(sendwin);

    mvwprintw(recvwin, next_msg_line++, 1, "Mensagens:");
    mvwprintw(sendwin, 1, 1, "$[%s] ", name);

//    wmove(recvwin, 5, 5);
//    leaveok(recvwin, TRUE);
    wrefresh(recvwin);
    wrefresh(sendwin);

    clearok(sendwin, TRUE);

    /* main loop: get and send lines of text */
    while (wscanw(sendwin, "%s", buf)) {

        mvwprintw(recvwin, next_msg_line++, 1, "[%s] %s\n", name, buf);

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

//        buf[MAX_LINE-1] = '\0';
//        len = strlen(buf) + 1;
//        send(s, buf, len, 0);
//        if (recv(s, buf, sizeof(buf), 0) > 0) {
//            fputs(buf, stdout);
//            fputs("\n", stdout);
//        }
    }

    close(s);
    endwin();

    return EXIT_SUCCESS;
}

/* Função para desenhar bordas nas janelas ncurses */
void draw_borders(WINDOW *screen) {
    int x, y;
    int i;

    getmaxyx(screen, y, x);

    /* Os quatro cantos */
    mvwprintw(screen, 0, 0, "+");
    mvwprintw(screen, y-1, 0, "+");
    mvwprintw(screen, 0, x-1, "+");
    mvwprintw(screen, y-1, x-1, "+");

    /* Os lados */
    for(i = 1; i < (y-1); i++) {
        mvwprintw(screen, i, 0, "|");
        mvwprintw(screen, i, x-1, "|");
    }

    /* Topo e base */
    for(i = 1; i < (x-1); i++) {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y-1, i, "-");
    }
}
