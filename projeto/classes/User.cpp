//
// Created by niko on 29/06/16.
//

#include "User.h"

User::User(string name, int socket){
    setName(name);
    setSocket(socket);

    login(socket);
}

bool User::login(int socket){
    if (getStatus()){
        sendFeedback(socket, "Login failed. User already connected.");
        return false;
    } else {
        setStatus(true);
        setSocket(socket);
        sendFeedback(socket, "Login Successful.");
    }
}

bool User::logout(int socket){
    setStatus(false);
    sendFeedback(socket, "Logout successful.");
}