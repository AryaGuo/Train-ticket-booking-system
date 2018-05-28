//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_USER_H
#define TRAIN_TICKET_BOOKING_SYSTEM_USER_H

#include <iostream>
#include "utility.hpp"

typedef sjtu::string wchar;

namespace sjtu
{

class user {

private:
    wchar name, pwd, email, phone;
    bool loginSta;

public:

    int id, priv;
    static int idCnt = 2017;

    user(): name(), pwd(), email(), phone() {
        id = -1;
        priv = 0;
        loginSta = false;
    }

    user(wchar const &name, wchar const &pwd, wchar const &email, wchar const &phone):
            name(name), pwd(pwd), email(email), phone(phone), id(-1), priv(0), loginSta(false) {}

    int getPri() {
        return priv;
    }
    std::ostream& operator<< (std::ostream &os, const user &obj) const {
        os << obj.name << obj.email << obj.phone << obj.priv;
        return os;
    }
    std::istream& operator>> (std::istream &is, user &obj) {
        is >> obj.name >> obj.pwd >> obj.email >> obj.phone;
        return is;
    }

};

}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_USER_H
