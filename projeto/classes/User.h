//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_USER_H
#define PROJETO_USER_H

#include <string>
#include <iostream>
#include <netinet/in.h>
#include <list>

using namespace std;

class User {
private:
    string m_name;
    bool m_status;
    list<string> m_messages;

    int m_socket;

public:
    User(string name, int socket);

    string getName() { return m_name; }
    bool getStatus() { return m_status; }
    list<string> getMessageList() { return m_messages; }
    int getSocket() { return m_socket; }

    void setName(string name) { m_name = name; }
    void setStatus(bool status) { m_status = status; }
    void setSocket(int socket) { m_socket = socket; }


    bool login(int socket);
    bool logout(int socket);

    void sendFeedback(int socket, string message);

};


#endif //PROJETO_USER_H
