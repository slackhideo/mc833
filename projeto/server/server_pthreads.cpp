#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#include <map>
#include <string>
#include <sstream>
#include <iostream>

#define MAX_PENDING 5
#define MAX_LINE 256

#define ever (; ;)

using namespace std;

typedef struct _params {
    int id;
    int new_s;
    map<string, pair<string, int> > *users;
    pthread_mutex_t mutex;
} params_t;

void *spawn_thread(void *params);

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    unsigned int len;
    int s, new_s;
    int num_threads = 0;
    pthread_t *threads;
    params_t *params;

    /* check parameter */
    if(argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(atoi(argv[1]));

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

    fprintf(stdout, "Server process ID : %d\n", getpid());

    /* users dictionary */
    map<string, pair<string, int> > users;

    /* wait for connection, then receive and print text */
    for ever {
        len = sizeof(sin);
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            perror("simplex-talk: accept");
            exit(1);
        }

        params = (params_t *)malloc(++num_threads * sizeof(params_t));
        params[num_threads-1].id = num_threads;
        params[num_threads-1].new_s = new_s;
        params[num_threads-1].users = &users;
        pthread_mutex_init(&params[num_threads-1].mutex, NULL);
        threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
        pthread_create(&threads[num_threads], NULL, spawn_thread, &params[num_threads-1]);
    }

    return EXIT_SUCCESS;
}

void *spawn_thread(void *params) {
    params_t *pars = (params_t *)params;
    struct sockaddr_in peer;
    unsigned int peerlen;
    char buf[MAX_LINE];
    int status;

    /* get client info */
    peerlen = sizeof(peer);
    if (getpeername(pars->new_s, (struct sockaddr *)&peer, &peerlen) < 0) {
        perror("simplex-talk: getpeername");
        exit(1);
    }

    /* build an ID for the user to use locally */
    stringstream userid_stream;
    string userid;
    userid_stream << inet_ntoa(peer.sin_addr) << ntohs(peer.sin_port);
    userid = userid_stream.str();

//            fprintf(stdout, "--------------------\n");
//            fprintf(stdout, "Remote IP: %s\n", inet_ntoa(peer.sin_addr));
//            fprintf(stdout, "Remote Port: %d\n", ntohs(peer.sin_port));
//            fprintf(stdout, "Process ID : %d\n", getpid());
//            fprintf(stdout, "Parent's ID: %d\n", getppid());
//            fprintf(stdout, "--------------------\n\n");

    /* get user's name */
    if(recv(pars->new_s, buf, sizeof(buf), 0) < 0) {
        perror("simplex-talk: recv");
        exit(1);
    }

    /* if user's name is not in the dictionary, include it */
    if((*pars->users).find(userid) == (*pars->users).end()) {
        (*pars->users).insert(make_pair(userid, make_pair(buf, 1)));
cout << "new user" << endl;
    }

    /* else set it to online */
    else {
        (*pars->users)[userid].second = 1;
cout << "old user" << endl;
    }

/* DEBUG */
fprintf(stdout,"username: %s (%s)\n", buf, userid.c_str());
cout << "size: " << (*pars->users).size() << endl;

    /* listen to the users command */
    while ((status = recv(pars->new_s, buf, sizeof(buf), 0))) {
        if(status < 0) {
            perror("simplex-talk: recv");
            continue;
        }
cout << "read thread " << pars->id << " socket " << pars->new_s << endl;

        /* evaluate command */
        string command(buf);
        stringstream output;

        /* WHO command */
        if(!command.compare("WHO")) {
            output << "From thread " << pars->id << endl;
            output << "| usuário  | status  |" << endl;

            /* get users and statuses */
            for(std::map<string, pair<string, int> >::iterator
                    it = (*pars->users).begin();
                    it != (*pars->users).end(); it++) {
                output << "| " << (it->second).first << " | ";
                if((it->second).second == 1) {
                    output << "online |" << endl;
                }
                else {
                    output << "offline |" << endl;
                }
            }

            strcpy(buf, output.str().c_str());
/* DEBUG */
fprintf(stdout,"output:\n%s\n", buf);
            if ((send(pars->new_s, buf, strlen(buf)+1, 0)) < 0) {
                perror("simplex-talk: send");
            }


        }

        /* EXIT command */
        else if(!command.compare("EXIT")) {
            (*pars->users)[userid].second = 0;
            if ((send(pars->new_s, "EXIT", 5, 0)) < 0) {
                perror("simplex-talk: send");
            }
        }
    }

    return NULL;
}
