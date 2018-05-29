//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
#define TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H

#include "utility.hpp"
#include "map.hpp"
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

namespace sjtu
{

    class train;

    string getTime(train const &tr, string date, string const &loc) {
        //TODO
    }

    double getPrice(train const &tr, string const &loc1, string const &loc2, int const &priceId) {
        //TODO
    }

    class train {

    public:
        string id, name;
        char catalog;
        int stationNum, priceNum;
        string* stationName; //[staionNum]
        string* priceName;   //[priceNum]
        time *arriveTime, *startTime, *stopover;    //[stationNum]
        double **price; //[stationNum] * [priceNum]

    public:
        train() {
            stationName = nullptr;
            arriveTime = startTime = stopover = nullptr;
            price = nullptr;
        }

        train& operator=(const train &other) {
            this->~train();
            id = other.id;
            name = other.name;
            catalog = other.catalog;
            stationNum = other.stationNum;
            priceNum = other.priceNum;
            stationName = new string[stationNum];
            priceName = new string[priceNum];
            arriveTime = new time[stationNum];
            startTime = new time[stationNum];
            stopover = new time[stationNum];
            price = new double* [stationNum];
            for(int i = 0; i < stationNum; ++i) {
                arriveTime[i] = other.arriveTime[i];
                startTime[i] = other.startTime[i];
                stopover[i] = other.stopover[i];
                price[i] = new double[priceNum];
                for(int j = 0; j < priceNum; ++j)
                    price[i][j] = other.price[i][j];
            }
            return *this;
        }

        bool operator < (const train &other) {
            return id < other.id;
        }

        string getId() const{
            return id;
        }

        ~train() {
            delete stationName;
            delete arriveTime;
            delete startTime;
            delete stopover;
            for(int i = 0; i < priceNum; i++)
                delete [] price[i];
            delete [] price;
        }
    };

    std::ostream &operator<<(std::ostream &os, const train &obj) {
        os << obj.id << ' ' << obj.name << ' ' << obj.catalog << ' ' << obj.stationNum << ' ' << obj.priceNum;
        for(int i = 0; i < obj.priceNum; ++i) {
            os << ' ' << obj.priceName[i];
        }
        os << std::endl;
        for(int i = 0; i < obj.stationNum; ++i) {
            os << obj.stationName[i] << ' ' << obj.arriveTime[i] << ' ' << obj.startTime[i] << ' ' << obj.stopover[i];
            for(int j = 0; j < obj.priceNum; ++j) {
                os << " ¥" << obj.price[i][j];
            }
            os << std::endl;
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, train &obj) {
        char tmp[20];
        is >> obj.id >> obj.name >> obj.catalog >> obj.stationNum >> obj.priceNum;
        obj.stationName = new string[obj.stationNum];
        obj.priceName = new string[obj.priceNum];
        obj.arriveTime = new time[obj.stationNum];
        obj.startTime = new time[obj.stationNum];
        obj.stopover = new time[obj.stationNum];
        obj.price = new double*[obj.stationNum];
        for(int i = 0; i < obj.priceNum; ++i) {
            is >> obj.priceName[i];
        }
        for(int i = 0; i < obj.stationNum; ++i) {
            is >> obj.stationName[i] >> obj.arriveTime[i] >> obj.startTime[i] >> obj.stopover[i];
            obj.price[i] = new double[obj.priceNum];
            for(int j = 0; j < obj.priceNum; ++j) {
                is >> tmp;
                obj.price[i][j] = stringToDouble(tmp);
            }
        }
        return is;
    }

}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
