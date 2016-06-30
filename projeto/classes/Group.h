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

};


#endif //PROJETO_GROUP_H
