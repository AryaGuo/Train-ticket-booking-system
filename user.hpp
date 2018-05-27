//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_USER_H
#define TRAIN_TICKET_BOOKING_SYSTEM_USER_H

#include <iostream>
#include "utility.hpp"
using namespace sjtu;

class user {

private:
    string name, pwd, email, phone;
    int id, priv;
    bool loginSta;

public:
    user(): name(), pwd(), email(), phone() {
        id = -1;
        priv = 0;
        loginSta = false;
    }

    user(string const &name, string const &pwd, string const &email, string const &phone, int &id): name(name), pwd(pwd), email(email), phone(phone), id(id), priv(priv) {}

    int pri() {
        return priv;
    }
    std::ostream& operator<< (std::ostream &os, const user &obj) const {
        os << obj.name << obj.email << obj.phone << obj.priv;
        return os;
    }
    std::istream& operator>> (std::istream &is, user &obj) {
        is >> obj.name >> obj.email >> obj.phone >> obj.priv;
        return is;
    }

};



#endif //TRAIN_TICKET_BOOKING_SYSTEM_USER_H
