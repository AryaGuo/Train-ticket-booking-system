//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_USER_H
#define TRAIN_TICKET_BOOKING_SYSTEM_USER_H

#include <iostream>
#include "utility.hpp"

namespace sjtu
{

    class user {

public:
    string name, pwd, email, phone;
    bool loginSta;

public:

    int id, priv;
    static int idCnt;

    user(): name(), pwd(), email(), phone() {
        id = -1;
        priv = 0;
        loginSta = false;
    }

    user(const user &other):
            id(other.id), priv(other.priv), name(other.name), pwd(other.pwd), email(other.email), phone(other.phone),
            loginSta(other.loginSta) {}

    user(string const &name, string const &pwd, string const &email, string const &phone):
            name(name), pwd(pwd), email(email), phone(phone), id(-1), priv(0), loginSta(false) {}

    int getPri() {
        return priv;
    }

    bool operator< (const user &other) const{
        return id < other.id;
    }

};
    std::ostream& operator<< (std::ostream &os, const user &obj) {
        os << obj.name << ' ' << obj.email << ' ' << obj.phone << ' ' << obj.priv;
        return os;
    }
    std::istream& operator>> (std::istream &is, user &obj) {
        is >> obj.name >> obj.pwd >> obj.email >> obj.phone;
        return is;
    }

    int user::idCnt = 2017;
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_USER_H
