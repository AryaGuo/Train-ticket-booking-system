//
// Created by Arya on 2018/6/6.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP
#define TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "utility.hpp"
#include "user.hpp"
#include "train.hpp"
#include "ticket.hpp"
#include "BPlusTree.h"
#include "tuple.hpp"
#include "map.hpp"
#include "queue.hpp"
#include "constant.h"

typedef sjtu::string string;
typedef sjtu::train train;
typedef sjtu::ticket ticket;
typedef sjtu::map <string, train> trainSet;
typedef sjtu::map <int, ticket> ticketSet; //TODO: ticketSet: orderId or ticket ??
typedef sjtu::user user;
typedef sjtu::time Time;

const int CAPACITY = sjtu::TRAIN_CAPACITY;

/*
 *  idUser      < id, user >

    sale        < trainId, train >
    nSale       < trainId, train >

    locTrain    < loc, trainSet >
    direct      < {loc1, loc2, catalog}, trainSet >
    transfer    < {loc1, loc2, catalog}, {time, train1, train2, loc0, loc1, loc2} >

    trKindTicket    < {trainId, date, kind}, ticketSet >
    trDate          < {trainId, date}, ticketSet >
    idTicket        < {id, date, catalog}, ticketSet >
    infoOrderId     < {id, trainId, loc1, loc2, date, kind}, OrderId >
    orderIdTicket   < OrderId, ticket >
 */

sjtu::BPlusTree <int, user> idUser("user");
sjtu::BPlusTree <string, train> sale("sale");
sjtu::BPlusTree <string, train> nSale("nsale");
sjtu::BPlusTree <string, trainSet> locTrain("locTrain");
sjtu::BPlusTree <sjtu::tuple3<string, string, char>, trainSet> direct("direct");
sjtu::BPlusTree <sjtu::tuple3<string, string, string>, ticketSet> trKindTicket("trKindTicket");
sjtu::BPlusTree <sjtu::pair<string, string>, ticketSet> trDate("trDate");
sjtu::BPlusTree <sjtu::tuple3<int, string, char>, ticketSet> idTicket("idTicket");
sjtu::BPlusTree <sjtu::tuple6<int, string, string, string, string, string>, int> infoOrderId("infoOrderId");
sjtu::BPlusTree <int, ticket> orderIdTicket("orderIdTicket");

namespace arya {

    bool intersect(train const &tr, ticket const &tic, string const &loc1, string const &loc2) {
        int l, r, s, t;
        for (int i = 0; i < tr.stationNum; ++i) {
            auto now = tr.stationName[i];
            if (now == loc1)
                l = i;
            if (now == loc2)
                r = i;
            if (now == tic.loc1)
                s = i;
            if (now == tic.loc2)
                t = i;
        }
        return !(r < s || l > t);
    }

    int Register(user &u) {
        int id = ++user::idCnt;
        u.id = id;
        if (id == 2018)
            u.priv = 2;
        if (idUser.insert(id, u))
            return id;
        return -1;
    }

    bool queryProfile(int const &id, user &u) {
        auto res = idUser.find(id);
        if (!res.first)
            return false;
        else {
            u = res.second;
            return true;
        }
    }

    bool login(int const &id, string const &pwd) {
        user u;
        if (!queryProfile(id, u))
            return false;
        if (u.pwd != pwd)
            return false;
        u.loginSta = true;
        idUser.modify(id, u);
        return true;
    }

    bool modifyProfile(int const &id, user &u) {
        auto res = idUser.find(id);
        if (!res.first)
            return false;
        u.id = res.second.id;
        u.priv = res.second.priv;
        idUser.modify(id, u);
        return true;
    }

    void modifyPrivilege(int const &id, user &u, int const &priv) {
        u.priv = priv;
        idUser.modify(id, u);
    }

    bool excludeTicket(train const &tr, string const &loc1, string const &loc2, string const &date,
                       sjtu::pair<train, sjtu::map<string, int>> &ret) {
        typedef sjtu::map<string, int> map;
        int sold = 0;
        map used;
        auto allTicket = trDate.find(sjtu::pair<string, string>(tr.getId(), date)).second;
        for (auto &j: allTicket) {
            ticket tic = j.second;
            if (intersect(tr, tic, loc1, loc2)) {
                if (used[tic.getKind()] += tic.getNum() == CAPACITY)
                    sold++;
            }
        }
        if (sold < tr.priceNum) {
            ret = sjtu::pair<train, map>(tr, used);
            return true;
        }
        return false;
    }

    void printQueryTicket(train const &tr, string const &loc1, string const &loc2, string const &date,
                          const sjtu::map<string, int> &used) {
        std::cout << tr.id << ' ' << loc1 << ' ' << sjtu::getStartTime(tr, date, loc1) << ' ' << loc2 << ' '
                  << sjtu::getArriveTime(tr, date, loc2);
        for (int i = 0; i < tr.priceNum; ++i) {
            int rem = CAPACITY;
            if (used.count(tr.priceName[i]))
                rem -= used[tr.priceName[i]];
            //TODO: rem == 0?
            if (rem > 0) {
                std::cout << ' ' << tr.priceName[i] << ' ' << rem << ' ' << sjtu::getPrice(tr, loc1, loc2, i);
            }
        }
        std::cout << std::endl;
    }

    bool queryTicket(string const &loc1, string const &loc2, string const &date, string const &catalog) {
        typedef sjtu::map<string, int> map;
        int len = catalog.length(), sum = 0;
        auto que = new sjtu::queue<sjtu::pair<train, map> >[len];
        for (int i = 0; i < len; ++i) {
            char cat = catalog[i];
            auto allTrain = direct.find(sjtu::tuple3<string, string, char>(loc1, loc2, cat)).second;
            for (auto &it: allTrain) {   //guarantees the order of trains
                train tr = it.second;
                /*int sold = 0;
                map used;
                auto allTicket = trDate.find(sjtu::pair<string, string>(tr.getId(), date)).second;
                for (auto &j: allTicket) {
                    ticket tic = j.second;
                    if(intersect(tr, tic, loc1, loc2)) {
                        if(used[tic.getKind()] += tic.getNum() == CAPACITY)
                            sold++;
                    }
                }
                if(sold < tr.priceNum) {
                    que[i].push(sjtu::pair<train, map>(tr, used));
                    sum++;
                }*/
                sjtu::pair<train, map> ret;
                if (excludeTicket(tr, loc1, loc2, date, ret)) {
                    que[i].push(ret);
                    sum++;
                }
            }
        }
        std::cout << sum << std::endl;
        while (sum--) {
            int pos = -1;
            for (int i = 0; i < len; ++i)
                if (!que[i].empty() && (pos == -1 || que[i].front().first.id < que[pos].front().first.id))
                    pos = i;
            auto tr = que[pos].front().first;
            auto used = que[pos].front().second;
            que[pos].pop();
            /*std::cout << tr.id << ' ' << loc1 << ' ' << sjtu::getStartTime(tr, date, loc1) << ' ' << loc2 << ' ' << sjtu::getArriveTime(tr, date, loc2);
            for(int i = 0; i < tr.priceNum; ++i) {
                int rem = CAPACITY - used[tr.priceName[i]];
                //TODO: rem == 0?
                if(rem > 0) {
                    std::cout << ' ' << tr.priceName[i] << ' ' << rem << ' ' << sjtu::getPrice(tr, loc1, loc2, i);
                }
            }*/
            printQueryTicket(tr, loc1, loc2, date, used);
        }
        return true;//TODO: -1?
    }

    Time calTime(train const &tr1, train const &tr2, string const &loc0, string const &loc1, string const &loc2) {
        Time t[4];
        for (int i = 0; i < tr1.stationNum; ++i) {
            if (tr1.stationName[i] == loc0) {
                t[0] = tr1.startTime[i];
            } else if (tr1.stationName[i] == loc1) {
                t[1] = tr1.arriveTime[i];
            }
        }
        for (int i = 0; i < tr2.stationNum; ++i) {
            if (tr2.stationName[i] == loc1) {
                t[2] = tr2.startTime[i];
            } else if (tr2.stationName[i] == loc2) {
                t[3] = tr2.arriveTime[i];
            }
        }
        Time res(0, 0), day(24, 0);
        for (int i = 0; i < 3; ++i)
            res += t[i] < t[i + 1] ? t[i + 1] - t[i] : t[i + 1] - t[i] + day;
        return res;
    }

    bool queryTransfer(string const &loc1, string const &loc2, string const &date, string const &catalog) {
        //loc1->loc0->loc2
        //time, loc1, loc0, loc2, tr1, tr2
        typedef sjtu::map<string, int> map;
        typedef sjtu::pair<train, map> trainInfo;
        typedef sjtu::tuple6<Time, string, string, string, trainInfo, trainInfo> Result;
        Result ans;
        bool flag = false;
        auto tmp = locTrain.find(loc1);
        if (!tmp.first)
            return false;
        auto trainS = tmp.second;
        for (auto &it: trainS) {
            auto tr1 = it.second;
            for (int i = tr1.stationNum - 1; i >= 0; --i) {
                auto loc0 = tr1.stationName[i];
                if (loc0 == loc1)
                    break;
                sjtu::pair<train, sjtu::map<string, int>> ret1, ret2;
                if (excludeTicket(tr1, loc1, loc0, date, ret1)) {
                    for (int pos = catalog.length() - 1; pos >= 0; pos--) {
                        char cat = catalog[pos];
                        auto tmp2 = direct.find(sjtu::tuple3<string, string, char>(loc0, loc1, cat));
                        if (tmp2.first) {
                            auto trainS2 = tmp2.second;
                            for (auto &it2: trainS2) {
                                auto tr2 = it2.second;
                                if (excludeTicket(tr2, loc0, loc2, date, ret2)) {
                                    if (!flag) {
                                        flag = true;
                                        ans = Result(calTime(tr1, tr2, loc1, loc0, loc2), loc1, loc0, loc2, ret1, ret2);
                                    } else {
                                        auto now = Result(calTime(tr1, tr2, loc1, loc0, loc2), loc1, loc0, loc2, ret1,
                                                          ret2);
                                        if (now.head() < ans.head())
                                            ans = now;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!flag)
            return false;
        printQueryTicket(ans.data5.first, ans.data2, ans.data3, date, ans.data5.second);
        printQueryTicket(ans.data6.first, ans.data3, ans.data4, date, ans.data6.second);
        return true;
    }

    void modifyTicketSet(int const &orderId, train const &tr, int const &id, int const &num, string const &trainId,
                         string const &date, string const &kind) {
        auto tic = orderIdTicket.find(orderId).second;
        tic.modifyNum(num);
        orderIdTicket.modify(orderId, tic);

        if (tic.num > 0) {
            auto t1 = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
            auto ticSet1 = t1.second;
            ticSet1[orderId] = tic;
            trKindTicket.modify(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);

            auto t2 = trDate.find(sjtu::pair<string, string>(trainId, date));
            auto ticSet2 = t2.second;
            ticSet2[orderId] = tic;
            trDate.modify(sjtu::pair<string, string>(trainId, date), ticSet2);

            auto t3 = idTicket.find(sjtu::tuple3<int, string, char>(id, date, tr.catalog));
            auto ticSet3 = t3.second;
            ticSet3[orderId] = tic;
            idTicket.modify(sjtu::tuple3<int, string, char>(id, date, tr.catalog), ticSet3);
        } else {
            auto t1 = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
            auto ticSet1 = t1.second;
            ticSet1.erase(ticSet1.find(orderId));
            trKindTicket.modify(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);

            auto t2 = trDate.find(sjtu::pair<string, string>(trainId, date));
            auto ticSet2 = t2.second;
            ticSet2.erase(ticSet1.find(orderId));
            trDate.modify(sjtu::pair<string, string>(trainId, date), ticSet2);

            auto t3 = idTicket.find(sjtu::tuple3<int, string, char>(id, date, tr.catalog));
            auto ticSet3 = t3.second;
            ticSet3.erase(ticSet1.find(orderId));
            idTicket.modify(sjtu::tuple3<int, string, char>(id, date, tr.catalog), ticSet3);
        }//TODO
    }

    void insertOrder(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                     string const &date, string const &kind) {
        auto key = sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind);
        auto tmp = infoOrderId.find(key);
        auto tr = sale.find(trainId).second;
        char catalog = tr.catalog;
        if (!tmp.first) {
            //create a new order
            int cnt = ++ticket::orderCnt;
            ticket newTic(id, cnt, num, trainId, loc1, loc2, date, kind, catalog);
            auto toInsert = sjtu::pair<int, ticket>(cnt, newTic);
            infoOrderId.insert(key, cnt);
            orderIdTicket.insert(cnt, newTic);

            auto t1 = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
            auto ticSet1 = t1.second;
            ticSet1.insert(toInsert);
            if (t1.first)
                trKindTicket.modify(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);
            else
                trKindTicket.insert(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);

            auto t2 = trDate.find(sjtu::pair<string, string>(trainId, date));
            auto ticSet2 = t2.second;
            ticSet2.insert(toInsert);
            if (t2.first)
                trDate.modify(sjtu::pair<string, string>(trainId, date), ticSet2);
            else
                trDate.insert(sjtu::pair<string, string>(trainId, date), ticSet2);

            auto t3 = idTicket.find(sjtu::tuple3<int, string, char>(id, date, catalog));
            auto ticSet3 = t3.second;
            ticSet3.insert(toInsert);
            if (t3.first)
                idTicket.modify(sjtu::tuple3<int, string, char>(id, date, catalog), ticSet3);
            else
                idTicket.insert(sjtu::tuple3<int, string, char>(id, date, catalog), ticSet3);
        } else {
            modifyTicketSet(tmp.second, tr, id, num, trainId, date, kind);
            /*
            auto orderId = tmp.second;
            auto tic = orderIdTicket.find(orderId).second;
            tic.modifyNum(num);
            orderIdTicket.modify(orderId, tic);

            auto t1 = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
            auto ticSet1 = t1.second;
            ticSet1[orderId] = tic;
            trKindTicket.modify(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);

            auto t2 = trDate.find(sjtu::pair<string, string>(trainId, date));
            auto ticSet2 = t2.second;
            ticSet2[orderId] = tic;
            trDate.modify(sjtu::pair<string, string>(trainId, date), ticSet2);

            auto t3 = idTicket.find(sjtu::tuple3<int, string, char>(id, date, catalog));
            auto ticSet3 = t3.second;
            ticSet3[orderId] = tic;
            idTicket.modify(sjtu::tuple3<int, string, char>(id, date, catalog), ticSet3);*/
        }
    }

    bool buyTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                   string const &date, string const &kind) {
        int used = 0;
        auto res = sale.find(trainId);
        if (!res.first)
            return false;
        train tr = res.second;
        auto tmp = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
        if (tmp.first) {
            auto allTicket = tmp.second;
            for (auto &it: allTicket) {
                auto ticket = it.second;
                if (intersect(tr, ticket, loc1, loc2)) {
                    used += ticket.getNum();
                }
            }
        }
        if (CAPACITY - used >= num) {
            insertOrder(id, num, trainId, loc1, loc2, date, kind);
            return true;
        }
        return false;
    }

    bool queryOrder(int const &id, string const &date, string const &catalog) {
        user u;
        if (!queryProfile(id, u))
            return false;
        int len = catalog.length(), sum = 0;
        sjtu::queue<ticketSet> que;
        for (int i = 0; i < len; ++i) {
            char cat = catalog[i];
            auto tmp = idTicket.find(sjtu::tuple3<int, string, char>(id, date, cat));
            if (!tmp.first)
                continue;
            sum += tmp.second.size();
            que.push(tmp.second);
        }
        std::cout << sum << std::endl;
        while (!que.empty()) {
            auto ticketSet = que.front();
            que.pop();
            for (auto &it: ticketSet) {
                auto tic = it.second;
                auto tr = sale.find(tic.trainId).second;
                std::cout << tic.trainId << ' ';
                std::cout << tic.loc1 << ' ' << sjtu::getStartTime(tr, date, tic.loc1) << ' ';
                std::cout << tic.loc2 << ' ' << sjtu::getArriveTime(tr, date, tic.loc2);
                for (int j = 0; j < tr.priceNum; ++j) {
                    std::cout << ' ' << tr.priceName[j] << ' ';
                    if (tr.priceName[j] == tic.kind)
                        std::cout << tic.num << ' ';
                    else
                        std::cout << "0 ";
                    std::cout << sjtu::getPrice(tr, tic.loc1, tic.loc2, j);
                }
                std::cout << std::endl;
            }
        }
        return true;
    }

    bool refundTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                      string const &date, string const &kind) {
        auto tmp = infoOrderId.find(
                sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind));
        if (!tmp.first)
            return false;
        auto orderId = tmp.second;
        auto tic = orderIdTicket.find(orderId).second;
        auto tr = sale.find(trainId).second;
        if (tic.num < num)
            return false;
        modifyTicketSet(orderId, tr, id, -num, trainId, date, kind);
        return true;
    }

    bool addTrain(train const &tr) {
        if (sale.find(tr.id).first || nSale.find(tr.id).first)
            return false;
        nSale.insert(tr.id, tr);
        return true;
    }

    bool saleTrain(string const &id) {
        auto tmp0 = nSale.find(id);
        if (!tmp0.first)
            return false;
        auto tr = tmp0.second;
        nSale.erase(id);
        sale.insert(id, tr);

        char cat = tr.catalog;
        for (int i = 0; i < tr.stationNum; ++i) {
            string loc1 = tr.stationName[i];
            for (int j = i + 1; j < tr.stationNum; ++j) {
                string loc2 = tr.stationName[j];
                auto key = sjtu::tuple3<string, string, char>(loc1, loc2, cat);
                auto tmp = direct.find(key);
                auto trainS = tmp.second;
                trainS.insert(sjtu::pair<string, train>(id, tr));
                if (tmp.first) {
                    direct.modify(key, trainS);
                } else {
                    direct.insert(key, trainS);
                }
            }
        }

        for (int i = 0; i < tr.stationNum; ++i) {
            string loc = tr.stationName[i];
            auto tmp = locTrain.find(loc);
            auto trainS = tmp.second;
            trainS.insert(sjtu::pair<string, train>(loc, tr));
            if (tmp.first) {
                locTrain.modify(loc, trainS);
            } else {
                locTrain.insert(loc, trainS);
            }
        }
        return true;
    }

    bool queryTrain(string const &id, train &T) {
        auto tmp = sale.find(id);
        if (tmp.first) {
            T = tmp.second;
            return true;
        }
        return false;
    }

    bool deleteTrain(string const &id) {
        if (!nSale.find(id).first)
            return false;
        nSale.erase(id);
        return true;
    }

    bool modifyTrain(train const &tr) {
        if (!nSale.find(tr.id).first)
            return false;
        nSale.modify(tr.id, tr);
        return true;
    }

}
#endif //TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP
