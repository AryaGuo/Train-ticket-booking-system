//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
#define TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H

#include "utility.hpp"
#include <iostream>

/*
     *  add_train *train_id* *name* *catalog* *num(station)* *num(price)* *(name(price) ) x
     num(price)*
     *[name time(arriv) time(startTime) time(stopover) (price) x num(price) ] x num(station)*
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

namespace sjtu {
    class Station {
        friend std::ostream &operator<<(std::ostream &os, const Station &obj);
        friend std::istream &operator>>(std::istream &is, Station &obj);

    public:
        int len;
        string name;
        time arrive, Start, stopover;
        double price[PRICE_NUM];

    public:
        Station() = default;
        Station(const int &len): len(len) {}

        string getName() {
            return name;
        }
        time getArrive() {
            return arrive;
        }
        time getStart() {
            return Start;
        }
        time getStopover() {
            return stopover;
        }
        double getPrice(const int &x) {
            return price[x];
        }
    };

    //TODO: check
    std::ostream &operator<<(std::ostream &os, const Station &obj)
    {
        os << obj.name << ' ' << obj.arrive << ' ' << obj.Start << ' ' << obj.stopover;
        for(int i = 0; i < obj.len; ++i) {
            os << " ￥" << obj.price[i];
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, Station &obj) {
        is >> obj.name >> obj.arrive >> obj.Start >> obj.stopover;
        for(int i = 0; i < obj.len; ++i) {
            string tmp;
            is >> tmp;
            obj.price[i] = stringToDouble(tmp);
        }
        return is;
    }

    class PriceName {
        friend std::ostream &operator<<(std::ostream &os, const PriceName &obj);
        friend std::istream &operator>>(std::istream &is, PriceName &obj);

    private:
        int len;
        string data[PRICE_NUM];

    public:
        PriceName() = default;

        PriceName(const int &len) : len(len) {}

        int length() const {
            return len;
        }

        string &operator[](const int &i) {
            if (i < 0 || i > len)
                throw sjtu::index_out_of_bound();
            return data[i];
        };

        string operator[](const int &i) const {
            if (i < 0 || i > len)
                throw sjtu::index_out_of_bound();
            return data[i];
        }
    };

    std::ostream &operator<<(std::ostream &os, const PriceName &obj) {
        for(int i = 0; i < obj.length(); ++i) {
            if(i)   os << ' ';
            os << obj[i];
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, PriceName &obj) {
        for(int i = 0; i < obj.length(); ++i)
            is >> obj[i];
        return is;
    }

    class train {
        friend std::ostream &operator<<(std::ostream &os, const train &obj);

        friend std::istream &operator>>(std::istream &is, train &obj);

    public:
        int stationNum, priceNum;
        int stationId[STATION_NUM];
        char catalog;
        string id, name;

    public:
        train() = default;

        train(const string &id) : id(id) {}

        bool operator<(const train &other) const{
            return id < other.id;
        }

        string getId() const {
            return id;
        }
    };

    std::ostream &operator<<(std::ostream &os, const train &obj) {
        os << obj.id << ' ' << obj.name << ' ' << obj.catalog << ' ' << obj.stationNum << ' ' << obj.priceNum;
        for (int i = 0; i < obj.priceNum; ++i);// os << ' ' << obj.priceName[i];
        return os;
    }

    std::istream &operator>>(std::istream &is, train &obj) {
        is >> obj.name >> obj.catalog >> obj.stationNum >> obj.priceNum;
        for (int i = 0; i < obj.priceNum; ++i);//is >> obj.priceName[i];
        return is;
    }



}

#endif //TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
