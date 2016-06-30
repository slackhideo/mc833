//
// Created by niko on 29/06/16.
//

#ifndef PROJETO_MESSAGE_H
#define PROJETO_MESSAGE_H

#include <iostream>
#include <string>
#include "User.h"

using namespace std;

enum msgStatus {sent, queued, delivered};

class Message {
private:
    hash<string> m_id;
    string m_message;
    User m_sender;
    User m_receiver;
    Group m_group;
    msgStatus m_status;

public:
    const hash<string> &getM_id() const {
        return m_id;
    }

    const string &getM_message() const {
        return m_message;
    }

    const User &getM_sender() const {
        return m_sender;
    }

    const User &getM_receiver() const {
        return m_receiver;
    }

    const Group &getM_group() const {
        return m_group;
    }

    msgStatus getM_status() const {
        return m_status;
    }
};


#endif //PROJETO_MESSAGE_H
