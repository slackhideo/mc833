//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_GROUP_H
#define PROJETO_GROUP_H

#include <iostream>
#include <string>
#include <list>
#include "User.h"
#include "Message.h"

using namespace std;

class Group {
private:
    string m_name;
    list<User> m_users;
    list<Message> m_messages;

public:
    Group(const string &m_name, const list<User> &m_users = {}, const list<Message> &m_messages = {}) : m_name(m_name),
                                                                                              m_users(m_users),
                                                                                              m_messages(m_messages) { }

    const string &getM_name() const {           return m_name;    }
    void setM_name(const string &m_name) {      Group::m_name = m_name;    }
    const list<User> &getM_users() const {      return m_users;    }

    void setM_users(const list<User> &m_users) {            Group::m_users = m_users;    }
    const list<Message> &getM_messages() const {            return m_messages;    }
    void setM_messages(const list<Message> &m_messages) {   Group::m_messages = m_messages;    }
};


#endif //PROJETO_GROUP_H
