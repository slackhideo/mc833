//
// Created by niko on 29/06/16.
//

#include "User.h"

bool User::login(int socket){
    if (isM_status()){
        UserFeedback("Login failed. User already connected.");
        return false;
    } else {
        setM_status(true);
        setM_socket(socket);
        UserFeedback("Login Successful.");

        readMessages();
        readFeedbackMessages();
    }
}

bool User::logout(int socket){
    setM_status(false);
    UserFeedback("Logout successful.");
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

void User::readFeedbackMessages() {

    while (!m_feedback.empty())
    {
        Message msg = m_feedback.front();

        if(msg.getM_status() == Sent){ // msg sent

            msg.setM_status(Queued);
            sendMessageFeedback(msg);

            cout << "Mensagem " << msg.getM_id() << " enviada!" << '\n';

        } else if(msg.getM_status() == Queued){ // msg queued

            msg.setM_status(Delivered);
            sendMessageFeedback(msg);

            cout << "Mensagem " << msg.getM_id() << " enfileirada!" << '\n';

        } else { // msg delivered

            cout << "Mensagem " << msg.getM_id() << " entregue!" << '\n';

        }

        m_feedback.pop_front();
    }
}