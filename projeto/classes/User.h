//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_USER_H
#define PROJETO_USER_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <list>
#include "Message.h"

using namespace std;

//class Message;

enum usrStatus {Offline, Online};

class User {
private:
    string m_name; // username
    usrStatus m_status;
    list<Message> m_messages; // unread messages
    list<Message> m_feedback; // feedback messages

    int m_socket;

public:
    User(const string &m_name, usrStatus m_status, int m_socket, const list<Message> &m_messages = {}, const list<Message> &m_feedback = {})
            : m_name(m_name), m_status(m_status), m_socket(m_socket), m_messages(m_messages), m_feedback(m_feedback) { }

    virtual ~User() { }

    const string &getM_name() const {                   return m_name;    }
    const list<Message> &getM_messages() const {        return m_messages;    }
    int getM_socket() const {                           return m_socket;    }
    const list<Message> &getM_feedback() const {        return m_feedback;    }

    void setM_name(const string &m_name) {                  User::m_name = m_name;    }
    void setM_status(usrStatus m_status) {                       User::m_status = m_status;    }
    void setM_messages(const list<Message> &m_messages) {   User::m_messages = m_messages;    }
    void setM_socket(int m_socket) {                        User::m_socket = m_socket;    }
    void setM_feedback(const list<Message> &m_feedback) {   User::m_feedback = m_feedback;    }

    bool isOnline() const;

    bool login(int socket);
    bool logout(int socket);

    void readMessages(); // when logging in, gets every message in m_messages
    void readFeedbackMessages(); // when logging in, gets every message in m_feedback

//    void sendMessageFeedback(Message message); // delivers to server message with updated status TODO
//    void UserFeedback(string message); // prints feedback messages to user  TODO

};


#endif //PROJETO_USER_H
