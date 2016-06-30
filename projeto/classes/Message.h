//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_MESSAGE_H
#define PROJETO_MESSAGE_H

#include <iostream>
#include <string>
#include <list>

#include "User.h"
#include "Group.h"

using namespace std;

enum msgStatus {Sent, Queued, Delivered};

class Message {
private:
    int m_id; // message_id
    string m_message; // text
    User m_sender; // sender object
    User m_receiver; // receiver object
    msgStatus m_status; // sent/queued/delivered

    Group m_group; // group
    list<User> m_groupRead; // list of users who read the message. Only used by groups.


public:


    Message(const string &m_message, const User &m_sender, const User &m_receiver) : m_message(m_message),
                                                                                     m_sender(m_sender),
                                                                                     m_receiver(m_receiver) {
        hash<string> hasher;
        auto hashed = hasher(getM_message());
        setM_id( (int)hashed % 10000000 );

        setM_status(Sent);
    }

    const int &getM_id() const {                 return m_id;    }
    const string &getM_message() const {         return m_message;    }
    const User &getM_sender() const {            return m_sender;    }
    const User &getM_receiver() const {          return m_receiver;    }
    const Group &getM_group() const {            return m_group;    }
    msgStatus getM_status() const {              return m_status;    }
    const list<User> &getM_groupRead() const {   return m_groupRead;    }

    void setM_id(const int &m_id) {                         Message::m_id = m_id;    }
    void setM_message(const string &m_message) {            Message::m_message = m_message;    }
    void setM_sender(const User &m_sender) {                Message::m_sender = m_sender;    }
    void setM_receiver(const User &m_receiver) {            Message::m_receiver = m_receiver;    }
    void setM_group(const Group &m_group) {                 Message::m_group = m_group;    }
    void setM_status(msgStatus m_status) {                  Message::m_status = m_status;    }
    void setM_groupRead(const list<User> &m_groupRead) {    Message::m_groupRead = m_groupRead;    }
};


#endif //PROJETO_MESSAGE_H
