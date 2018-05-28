//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
#define TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H

#include "utility.hpp"
#include <iostream>

typedef sjtu::string wchar;
typedef sjtu::time time;

/*
     *  add_train *train_id* *name* *catalog* *num(station)* *num(price)* *(name(price) ) x
     num(price)*
     *[name time(arriv) time(startTime) time(stopoverover) (price) x num(price) ] x num(station)*
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

    double stringToDouble(wchar str) {
        int len = str.length(), pos = 1;
        double res = 0, base = 1;
        while(str[pos] != '.' && pos < len)
            res = res * 10 + str[pos] - '0', ++pos;
        ++pos;
        while(pos < len) {
            base /= 10;
            res += (str[pos] - '0') * base;
            ++pos;
        }
        return res;
    }

    class train {
    private:
        wchar id, name, catalog;
        int stationNum, priceNum;
        wchar* stationName; //[staionNum]
        wchar* priceName;   //[priceNum]
        time *arriveTime, *startTime, *stopover;    //[stationNum]
        double **price; //[stationNum] * [priceNum]

    public:
        train() {
            stationName = nullptr;
            arriveTime = startTime = stopover = nullptr;
            price = nullptr;
        }

        std::ostream& operator<< (std::ostream &os, const train &obj) const;
        std::istream& operator>> (std::istream &is, train &obj);

        wchar getId() const{
            return id;
        }

        ~train() {
            delete stationName;
            delete arriveTime;
            delete startTime;
            delete stopover;
            for(int i = 0; i < priceNum; i++)
                delete price[i];
            delete []price;
        }
    };

    std::ostream &train::operator<<(std::ostream &os, const train &obj) const {
        os << id << ' ' << name << ' ' << catalog << ' ' << stationNum << ' ' << priceNum;
        for(int i = 0; i < priceNum; ++i) {
            os << ' ' << priceName[i];
        }
        os << std::endl;
        for(int i = 0; i < stationNum; ++i) {
            os << stationName[i] << ' ' << arriveTime[i] << ' ' << startTime[i] << ' ' << stopover[i];
            for(int j = 0; j < priceNum; ++j) {
                os << " ¥" << price[i][j];
            }
            os << std::endl;
        }
        return os;
    }

    std::istream &train::operator>>(std::istream &is, train &obj) {
        char tmp[20];
        is >> id >> name >> catalog >> stationNum >> priceNum;
        stationName = new wchar[stationNum];
        priceName = new wchar[priceNum];
        arriveTime = new time[stationNum];
        startTime = new time[stationNum];
        stopover = new time[stationNum];
        for(int i = 0; i < priceNum; ++i) {
            is >> priceName[i];
        }
        for(int i = 0; i < stationNum; ++i) {
            is >> stationName[i] >> arriveTime[i] >> startTime[i] >> stopover[i];
            for(int j = 0; j < priceNum; ++j) {
                is >> tmp;
                price[i][j] = stringToDouble(tmp);
            }
        }
        return is;
    }
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
