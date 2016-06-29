#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <map>
#include <string>
#include <sstream>
#include <iostream>

#define MAX_PENDING 5
#define MAX_LINE 256

#define ever (; ;)

using namespace std;

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct sockaddr_in peer;
    char buf[MAX_LINE];
    unsigned int len;
    int status;
    unsigned int peerlen;
    int s, new_s;
    int pid;

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

        /* child code */
        if((pid = fork()) == 0) {
            close(s);

            /* get client info */
            peerlen = sizeof(peer);
            if (getpeername(new_s, (struct sockaddr *)&peer, &peerlen) < 0) {
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
            if(recv(new_s, buf, sizeof(buf), 0) < 0) {
                perror("simplex-talk: recv");
                exit(1);
            }

            /* if user's name is not in the dictionary, include it */
            if(users.find(userid) == users.end()) {
                users.insert(make_pair(userid, make_pair(buf, 1)));
cout << "new user" << endl;
            }

            /* else set it to online */
            else {
                users[userid].second = 1;
cout << "old user" << endl;
            }

/* DEBUG */
fprintf(stdout,"username: %s (%s)\n", buf, userid.c_str());
cout << "size: " << users.size() << endl;

            /* listen to the users command */
            while ((status = recv(new_s, buf, sizeof(buf), 0))) {
                if(status < 0) {
                    perror("simplex-talk: recv");
                    continue;
                }

                /* evaluate command */
                string command(buf);
                stringstream output;

                /* WHO command */
                if(!command.compare("WHO")) {
                    output << "| usuário  | status  |" << endl;

                    /* get users and statuses */
                    for(std::map<string, pair<string, int> >::iterator
                            it = users.begin();
                            it != users.end(); it++) {
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
                    if ((send(new_s, buf, strlen(buf)+1, 0)) < 0) {
                        perror("simplex-talk: send");
                    }


                }

                /* EXIT command */
                else if(!command.compare("EXIT")) {
                    users[userid].second = 0;
                }
            }

            //close child connection
            close(new_s);
            exit(0);
        }

        //close parent connection (the child owns this socket now)
        close(new_s);
    }

    return EXIT_SUCCESS;
}
