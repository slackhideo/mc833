//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_USER_H
#define PROJETO_USER_H

#include <iostream>
#include <string>
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


    const string &getM_name() const {
        return m_name;
    }

    bool isM_status() const {
        return m_status;
    }

    const list<string> &getM_messages() const {
        return m_messages;
    }

    int getM_socket() const {
        return m_socket;
    }


    void setM_name(const string &m_name) {
        User::m_name = m_name;
    }

    void setM_status(bool m_status) {
        User::m_status = m_status;
    }

    void setM_messages(const list<string> &m_messages) {
        User::m_messages = m_messages;
    }

    void setM_socket(int m_socket) {
        User::m_socket = m_socket;
    }

    bool login(int socket);
    bool logout(int socket);

    void sendFeedback(int socket, string message);

};


#endif //PROJETO_USER_H
