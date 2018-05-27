//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
#define TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H

#include "utility.hpp"
using namespace sjtu;

/*
     *  add_train *train_id* *name* *catalog* *num(station)* *num(price)* *(name(price) ) x
     num(price)*
     *[name time(arriv) time(start) time(stopover) (price) x num(price) ] x num(station)*
    返回值
    1 or 0
    说明
    添加车号为train_id，名为name的车，该车经过num(station)站(<=60)，共有num(price)种票价(<=5)， 并在之后一一列出是哪种票。
                     接下来的num(station)行给出各个车站相关信息。 注意刚刚新建的车次中所售车票 query_ticket
    能被查询或购买。 样例
    add_train abc123456 G123456 G 2 1 商务座 北京 xx:xx 08:00 00:00 ¥0.0
    夏威夷 08:01 xx:xx 00:00 ¥1.5
    ->; 1
     */
class train {

private:
    string id, name, catalog;
    int stationNum, priceNum;
    string* stationName;
    time *arrive, *start, *stop;
    double **price;

public:
    train() {
        stationName = nullptr;
        arrive = start = stop = nullptr;
        price = nullptr;
    }

    std::ostream& operator<< (std::ostream &os, const train &obj) const;
    std::istream& operator>> (std::istream &is, train &obj);

    ~train() {
        delete stationName;
        delete arrive;
        delete start;
        delete stop;
        for(int i = 0; i < priceNum; i++)
            delete price[i];
        delete []price;
    }
};


#endif //TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
