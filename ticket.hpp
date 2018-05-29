//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TICKET_H
#define TRAIN_TICKET_BOOKING_SYSTEM_TICKET_H

#include "utility.hpp"
#include "train.hpp"

namespace sjtu
{

    class ticket {
    public:
        int userId, orderId, num;
        string trainId, loc1, loc2, date, kind, catalog;

    public:
        static int orderCnt;

        ticket() {}

        ticket(int userId, int orderId, int num, string trainId, string loc1, string loc2, string date, string kind,
               string catalog)
                : userId(userId), orderId(orderId), num(num), trainId(trainId), loc1(loc1), loc2(loc2), date(date),
                  kind(kind), catalog(catalog) {}

        bool operator<(const ticket &other) {
            return orderId < other.orderId;
        }

        void modifyNum(const int &add) {
            num += add;
        }

        int getNum() {
            return num;
        }

        string getKind() {
            return kind;
        }

    };

    int ticket::orderCnt = 0;
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_TICKET_H
