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
        sendFeedback(socket, "Login failed. User already connected.");
        return false;
    } else {
        setM_status(true);
        setM_socket(socket);
        sendFeedback(socket, "Login Successful.");
    }
}

bool User::logout(int socket){
    setM_status(false);
    sendFeedback(socket, "Logout successful.");
}