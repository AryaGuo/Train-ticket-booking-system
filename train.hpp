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

    class train {

    public:
        string id, name;
        char catalog;
        int stationNum, priceNum;
        string stationName[STATION_NUM]; //[stationNum]
        string priceName[PRICE_NUM];   //[priceNum]
        time arriveTime[STATION_NUM], startTime[STATION_NUM], stopover[STATION_NUM];    //[stationNum]
        double price[STATION_NUM][PRICE_NUM]; //[stationNum] * [priceNum]

    public:
        train():stationNum(0), priceNum(0) {}

        train(const train &other):
                id(other.id),name(other.name), catalog(other.catalog), stationNum(other.stationNum), priceNum(other.priceNum)  {
            for(int i = 0; i < stationNum; ++i) {
                stationName[i] = other.stationName[i];
                arriveTime[i] = other.arriveTime[i];
                startTime[i] = other.startTime[i];
                stopover[i] = other.stopover[i];
                for(int j = 0; j < priceNum; ++j)
                    price[i][j] = other.price[i][j];
            }
            for(int i = 0; i < priceNum; ++i) {
                priceName[i] = other.priceName[i];
            }
        }

        train& operator=(const train &other) {
            if(this == &other)
                return *this;
            id = other.id;
            name = other.name;
            catalog = other.catalog;
            stationNum = other.stationNum;
            priceNum = other.priceNum;
            for(int i = 0; i < stationNum; ++i) {
                stationName[i] = other.stationName[i];
                arriveTime[i] = other.arriveTime[i];
                startTime[i] = other.startTime[i];
                stopover[i] = other.stopover[i];
                for(int j = 0; j < priceNum; ++j)
                    price[i][j] = other.price[i][j];
            }
            for(int i = 0; i < priceNum; ++i) {
                priceName[i] = other.priceName[i];
            }
            return *this;
        }

        bool operator < (const train &other) {
            return id < other.id;
        }

        string getId() const{
            return id;
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
        string tmp;
        is >> obj.id >> obj.name >> obj.catalog >> obj.stationNum >> obj.priceNum;
        for(int i = 0; i < obj.priceNum; ++i) {
            is >> obj.priceName[i];
        }
        for(int i = 0; i < obj.stationNum; ++i) {
            is >> obj.stationName[i] >> obj.arriveTime[i] >> obj.startTime[i] >> obj.stopover[i];
            for(int j = 0; j < obj.priceNum; ++j) {
                is >> tmp;
                obj.price[i][j] = stringToDouble(tmp);
            }
        }
        return is;
    }

    /*
     * date + time
     * len = 16
     * 2018-03-28 08:00
     */
    string getStartTime(train const &tr, string date, string const &loc) {
        string res(date);
        res += ' ';
        for(int i = 0; i < tr.stationNum; ++i)
            if(tr.stationName[i] == loc) {
                return res += tr.startTime[i].out();
            }
            //TODO
    }

    string getArriveTime(train const &tr, string date, string const &loc) {
        string res(date);
        res += ' ';
        for(int i = 0; i < tr.stationNum; ++i)
            if(tr.stationName[i] == loc) {
                return res += tr.arriveTime[i].out();
            }
            //TODO
    }

    double getPrice(train const &tr, string const &loc1, string const &loc2, int const &priceId) {
        int pos1 = 0, pos2 = 0;
        double sum = 0;
        for(int i = 0; i < tr.stationNum; ++i) {
            if(tr.stationName[i] == loc1)
                pos1 = i;
            if(tr.stationName[i] == loc2)
                pos2 = i;
        }
        for(int i = pos1 + 1; i <= pos2; i++) {
            sum += tr.price[i][priceId];
        }
        return sum;
    }

}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_TRAIN_H
