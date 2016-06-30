//
// Created by niko on 29/06/16.
//

#include "User.h"

User::User(string name, int socket){
    setM_name(name);
    setM_socket(socket);

    login(socket);
}

bool User::login(int socket){
    if (isM_status()){
        UserFeedback(socket, "Login failed. User already connected.");
        return false;
    } else {
        setM_status(true);
        setM_socket(socket);
        UserFeedback(socket, "Login Successful.");

        readMessages();
    }
}

bool User::logout(int socket){
    setM_status(false);
    UserFeedback(socket, "Logout successful.");
}

void User::readMessages(){

    while (!m_messages.empty())
    {
        Message msg = m_messages.front();

        cout << '[' << msg.getM_sender().getM_name() << ">] ";
        cout << msg.getM_message() << '\n';

        msg.setM_status(Delivered);
        sendMessageFeedback(msg);

        m_messages.pop_front();
    }
}
