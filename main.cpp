//
// Created by Arya on 2018/5/11.
//

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
typedef sjtu::map <string, ticket> ticketSet;
typedef sjtu::user user;
typedef sjtu::time Time;

const int CAPACITY = sjtu::TRAIN_CAPACITY;

/*
 * idUser      < id, user >

    sale        < trainId, train >	sale
    nSale       < trainId, train >	notSale

    locTrain    < loc, trainSet >
    direct      < {loc1, loc2, catalog}, trainSet >	direct
    transfer    < {loc1, loc2, catalog}, {time, train1, train2} >	transfer

    trKindTicket    < {trainId, date, kind}, ticketSet >
    trCatTicket     < {trainId, date, catalog}, ticketSet >
    idTicket        < {id, date, catalog}, ticketSet >
    infoOrderId     < {id, trainId, loc1, loc2, date, kind}, OrderId >
    orderIdTicket   < OrderId, ticket >
 */

sjtu::BPlusTree <int, user> idUser("user");
sjtu::BPlusTree <string, train> sale("sale");
sjtu::BPlusTree <string, train> nSale("nsale");
sjtu::BPlusTree <string, sjtu::map<string, train> > locTrain("locTrain");
sjtu::BPlusTree <sjtu::tuple3<string, string, char>, trainSet> direct("direct");
sjtu::BPlusTree <sjtu::tuple3<string, string, char>, sjtu::tuple3<Time, train, train> > transfer("transfer");
sjtu::BPlusTree <sjtu::tuple3<string, string, string>, ticketSet> trKindTicket("trKindTicket");
sjtu::BPlusTree <sjtu::tuple3<string, string, char>, ticketSet> trCatTicket("trCatTicket");
sjtu::BPlusTree <sjtu::tuple3<int, string, char>, ticketSet> idTicket("idTicket");
sjtu::BPlusTree <sjtu::tuple6<int, string, string, string, string, string>, int> infoOrderId("infoOrderId");
sjtu::BPlusTree <int, ticket> orderIdTicket("orderIdTicket");

bool intersect(ticket const &tic, string const &loc1, string const &loc2)
{
    train tr = sale.find(tic.trainId).second;
    int l, r, s, t;
    for(int i = 0; i < tr.stationNum; ++i) {
        auto now = tr.stationName[i];
        if(now == loc1)
            l = i;
        else if(now == loc2)
            r = i;
        else if(now == tic.loc1)
            s = i;
        else if(now == tic.loc2)
            t = i;
    }
    return !(r < s || l > t);
}

int Register(user &u)
{
    int id = ++user::idCnt;
    u.id = id;
    if(id == 2018)
        u.priv = 2;
    idUser.insert(id, u);
    return 1;
}

bool queryProfile(int const &id, user &u)
{
    auto res = idUser.find(id);
    if(!res.first)
        return false;
    else {
        u = res.second;
        return true;
    }
}

bool login(int const &id, string const &pwd)
{
    user u;
    if(!queryProfile(id, u))
        return false;
    if(u.pwd != pwd)
        return false;
    u.loginSta = true;
    idUser.modify(id, u);
    return true;
}

bool modifyProfile(int const &id, user &u)  //u with name, pwd, email, phone.
{
    auto pre = idUser.find(id);
    if(!pre.first)
        return false;
    u.id = pre.second.id;
    u.priv = pre.second.priv;
    idUser.modify(id, u);
    return true;
}

void modifyPrivilege(int const &id, int const &priv)
{
    auto pre = idUser.find(id).second;
    pre.priv = priv;
    idUser.modify(id, pre);
}

void queryTicket(string const &loc1, string const &loc2, string const &date, string const &catalog)
{
    typedef sjtu::map <string, int> map;
    int len = catalog.length();
    sjtu::queue <sjtu::pair<train, map > > que;
    for(int i = 0; i < len; ++i) {
        char cat = catalog[i];
        auto allTrain = direct.find(sjtu::tuple3<string, string, char>(loc1, loc2, cat)).second;
        for (auto &it: allTrain) {   //guarantees the order of trains
            train tr = it.second;
            int sold = 0;
            map used;
            auto allTicket = trCatTicket.find(sjtu::tuple3<string, string, char>(tr.getId(), date, cat)).second;
            for (auto &j: allTicket) {
                ticket tic = j.second;
                if(intersect(tic, loc1, loc2)) {
                    if(used[tic.getKind()] += tic.getNum() == CAPACITY)
                        sold++;
                }
            }
            if(sold < tr.priceNum) {
                que.push(sjtu::pair<train, map >(tr, used));
            }
        }
    }
    std::cout << que.size() << std::endl;
    while(!que.empty()) {
        auto tr = que.front().first;
        auto used = que.front().second;
        que.pop();
        std::cout << tr.id << ' ' << loc1 << ' ' << sjtu::getStartTime(tr, date, loc1) << ' ' << loc2 << sjtu::getArriveTime(tr, date, loc2);
        for(int i = 0; i < tr.priceNum; ++i) {
            int rem = CAPACITY - used[tr.priceName[i]];
            //TODO: rem == 0?
            if(rem > 0) {
                std::cout << ' ' << tr.priceName[i] << ' ' << rem << ' ' << sjtu::getPrice(tr, loc1, loc2, i);
            }
            std::cout << std::endl;
        }
    }
}

void queryTransfer(string const &loc1, string const &loc2, string const &date, string const &catalog)
{
    typedef sjtu::map <string, int> map;
    int len = catalog.length();
    sjtu::queue <sjtu::pair<train, map > > que;
    for(int i = 0; i < len; ++i) {
        char cat = catalog[i];
        auto trainPairs = transfer.find(sjtu::tuple3<string, string, char>(loc1, loc2, cat)).second;
        train tr1 = trainPairs.data2, tr2 = trainPairs.data3;
        //TODO
    }
}

void insertOrder(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2, string const &date, string const &kind)
{
    auto key = sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind);
    auto tmp = infoOrderId.find(key);
    if(!tmp.first) {
        //create a new order
        int cnt = ++ticket::orderCnt;
        auto tr = sale.find(trainId).second;
        char catalog = tr.catalog;
        ticket newTic(id, cnt, num, trainId, loc1, loc2, date, kind, catalog);
        auto toInsert = sjtu::pair<string, ticket>(trainId ,newTic); //TODO: ticketSet: orderId or ticket ??
        orderIdTicket.insert(cnt, newTic);

        auto ticSet1 = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind)).second;
        ticSet1.insert(toInsert);
        trKindTicket.modify(sjtu::tuple3<string, string, string>(trainId, date, kind), ticSet1);

        auto ticSet2 = trCatTicket.find(sjtu::tuple3<string, string, char>(trainId, date, catalog)).second;
        ticSet2.insert(toInsert);
        trCatTicket.modify(sjtu::tuple3<string, string, char>(trainId, date, catalog), ticSet2);

        auto ticSet3 = idTicket.find(sjtu::tuple3<int, string, char>(id, date, catalog)).second;
        ticSet3.insert(toInsert);
        idTicket.modify(sjtu::tuple3<int, string, char>(id, date, catalog), ticSet3);

        infoOrderId.insert(sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind), cnt);
    }
    else {
        auto orderId = tmp.second;
        auto tic = orderIdTicket.find(orderId).second;
        tic.modifyNum(num);
        orderIdTicket.modify(orderId, tic);
    }
}

bool buyTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2, string const &date, string const &kind) {
    int used = 0;
    auto tmp = trKindTicket.find(sjtu::tuple3<string, string, string>(trainId, date, kind));
    if(tmp.first) {
        auto allTicket = tmp.second;
        for (auto &it: allTicket) {
            auto ticket = it.second;
            if(intersect(ticket, loc1, loc2)) {
                used += ticket.getNum();
            }
        }
    }
    if(used < CAPACITY) {
        insertOrder(id, num, trainId, loc1, loc2, date, kind);
        return true;
    }
    return false;
}

bool queryOrder(int const &id, string const &date, string const &catalog)
{
    bool flag = false;
    int len = catalog.length(), sum = 0;
    sjtu::queue<ticketSet> que;
    for(int i = 0; i < len; ++i) {
        char cat = catalog[i];
        auto tmp = idTicket.find(sjtu::tuple3<int, string, char>(id, date, cat));
        if(!tmp.first)
            continue;
        flag = true;
        sum += tmp.second.size();
        que.push(tmp.second);
    }
    std::cout << sum << std::endl;
    while(!que.empty()) {
        auto ticketSet = que.front();
        que.pop();
        for (auto &it: ticketSet) {
            auto tic = it.second;
            auto tr = sale.find(tic.trainId).second;
            std::cout << tic.trainId << ' ';
            std::cout << tic.loc1 << ' ' << sjtu::getStartTime(tr, date, tic.loc1) << ' ';
            std::cout << tic.loc2 << ' ' << sjtu::getArriveTime(tr, date, tic.loc2) << ' ';
            for(int j = 0; j < tr.priceNum; ++j) {
                std::cout << tr.priceName[j] << ' ';
                if(tr.priceName[j] == tic.kind)
                    std::cout << tic.num << ' ';
                else
                    std::cout << "0 ";
                std::cout << sjtu::getPrice(tr, tic.loc1, tic.loc2, j) << ' ';
            }
            std::cout << std::endl;
        }
    }
    return flag;
}

bool refundTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2, string const &date, string const &kind)
{
    auto tmp = infoOrderId.find(sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind));
    if(!tmp.first)
        return false;
    auto orderId = tmp.second;
    auto tic = orderIdTicket.find(orderId).second;
    if(tic.num < num)
        return false;
    tic.modifyNum(-num);
    orderIdTicket.modify(orderId, tic);
    //TODO
    return true;
}

bool addTrain(train const &tr)
{
    if(sale.find(tr.id).first || nSale.find(tr.id).first)
        return false;
    nSale.insert(tr.id, tr);
    return true;
}

Time calTime(train const &tr1, train const &tr2, string const &loc0, string const &loc1, string const &loc2)
{
    Time t[4];
    for(int i = 0; i < tr1.stationNum; ++i) {
        if(tr1.stationName[i] == loc0) {
            t[0] = tr1.startTime[i];
        }
        else if(tr1.stationName[i] == loc1) {
            t[1] = tr1.arriveTime[i];
        }
    }
    for(int i = 0; i < tr2.stationNum; ++i) {
        if(tr2.stationName[i] == loc1) {
            t[2] = tr2.startTime[i];
        }
        else if(tr2.stationName[i] == loc2) {
            t[3] = tr2.arriveTime[i];
        }
    }
    Time res(0, 0), day(24, 0);
    for(int i = 0; i < 3; ++i)
        res += t[i] < t[i + 1] ? t[i + 1] - t[i] : t[i + 1] - t[i] + day;
    return res;
}

bool saleTrain(string const &id) {
    auto tmp0 = nSale.find(id);
    auto tr = tmp0.second;
    if (!tmp0.first)
        return false;
    nSale.erase(id);
    sale.insert(id, tr);

    char catalog = tr.catalog;
    for (int i = 0; i < tr.stationNum; ++i) {
        string loc1 = tr.stationName[i];
        for (int j = i + 1; j < tr.stationNum; ++j) {
            string loc2 = tr.stationName[j];
            auto key = sjtu::tuple3<string, string, char>(loc1, loc2, catalog);
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
        string loc1 = tr.stationName[i];
        auto trainS = locTrain.find(loc1).second;
        for (auto &it: trainS) {
            auto tr0 = it.second;
            for (int k = 0; k < tr0.stationNum; ++k) {
                string loc0 = tr0.stationName[k];
                if (loc0 == loc1) break;
                for (int j = i + 1; j < tr.stationNum; ++j) {
                    string loc2 = tr.stationName[j];
                    auto key = sjtu::tuple3<string, string, char>(loc0, loc2, catalog);
                    auto tmp = transfer.find(key);
                    if (!tmp.first) {
                        transfer.insert(key,
                                        sjtu::tuple3<Time, train, train>(calTime(tr0, tr, loc0, loc1, loc2), tr0, tr));
                    } else {
                        auto minTime = tmp.second.head();
                        auto now = calTime(tr0, tr, loc0, loc1, loc2);
                        if (now < minTime) {
                            transfer.modify(key, sjtu::tuple3<Time, train, train>(now, tr0, tr));
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < tr.stationNum; ++i) {
        string loc1 = tr.stationName[i];
        for (int j = i + 1; j < tr.stationNum; ++j) {
            string loc2 = tr.stationName[j];
            auto trainS = locTrain.find(loc2).second;
            for (auto &it: trainS) {
                auto tr1 = it.second;
                for (int k = tr1.stationNum; k > 0; --k) {
                    string loc3 = tr1.stationName[k];
                    if (loc3 == loc1) break;
                    auto key = sjtu::tuple3<string, string, char>(loc1, loc3, catalog);
                    auto tmp = transfer.find(key);
                    if (!tmp.first) {
                        transfer.insert(key,
                                        sjtu::tuple3<Time, train, train>(calTime(tr, tr1, loc1, loc2, loc3), tr, tr1));
                    } else {
                        auto minTime = tmp.second.head();
                        auto now = calTime(tr, tr1, loc1, loc2, loc3);
                        if (now < minTime) {
                            transfer.modify(key, sjtu::tuple3<Time, train, train>(now, tr, tr1));
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < tr.stationNum; ++i) {
        string loc = tr.stationName[i];
        for (int j = i + 1; j < tr.stationNum; ++j) {
            auto key = loc;
            auto tmp = locTrain.find(key);
            auto trainS = tmp.second;
            trainS.insert(sjtu::pair<string, train>(key, tr));
            if (tmp.first) {
                locTrain.modify(key, trainS);
            } else {
                locTrain.insert(key, trainS);
            }
        }
    }
    return true;
}

bool queryTrain(string const &id, train &T)
{
    auto tmp1 = sale.find(id);
    if(tmp1.first) {
        T = tmp1.second;
        return true;
    }
    auto tmp2 = nSale.find(id);
    if(tmp2.first) {
        T = tmp2.second;
        return true;
    }
    return false;
}

bool deleteTrain(string const &id)
{
    auto tmp = nSale.find(id);
    if(!tmp.first)
        return false;
    nSale.erase(id);
    return true;
}

bool modifyTrain(train const &tr)
{
    if(nSale.find(tr.id).first)
        return false;
    nSale.modify(tr.id, tr);
    return true;
}

void Register()
{
    user u;
    std::cin >> u;
    std::cout << Register(u) << std::endl;
}

void login()
{
    int id;
    string pwd;
    std::cin >> id >> pwd;
    std::cout << login(id, pwd) << std::endl;
}

void queryProfile()
{
    int id;
    user u;
    std::cin >> id;
    if(!queryProfile(id, u))
        std::cout << "0" << std::endl;
    else
        std::cout << u << std::endl;
}

void modifyProfile()
{
    int id;
    user u;
    std::cin >> id >> u;
    if(!modifyProfile(id, u))
        std::cout << "0" << std::endl;
    else
        std::cout << "1" << std::endl;
}

void modifyPrivilege()
{
    int id1, id2, priv;
    user u1, u2;
    std::cin >> id1 >> id2 >> priv;
    if(!queryProfile(id1, u1) || !queryProfile(id2, u2) || u1.getPri() !=2 || u2.getPri() > priv)
        std::cout << "0" << std::endl;
    else
    {
        modifyPrivilege(id2, priv);
        std::cout << "1" << std::endl;
    }
}

void queryTicket()
{
    string loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    queryTicket(loc1, loc2, date, catalog);
}

void queryTransfer()
{
    string loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    queryTransfer(loc1, loc2, date, catalog);
}

void buyTicket()
{
    // buy_ticket *id* *num* *train_id* *loc1* *loc2* *date* *ticket_kind*
    string trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    std::cout << buyTicket(id, num, trainId, loc1, loc2, date, kind) << std::endl;
}

void queryOrder()
{
    //query_order *id* *date* *catalog*
    int id;
    string date, catalog;
    user u;
    std::cin >> id >> date >> catalog;
    if(!queryOrder(id, date, catalog))
        std::cout << "-1" << std::endl;
}

void refundTicket()
{
    string trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    std::cout << refundTicket(id, num, trainId, loc1, loc2, date, kind) << std::endl;
}

void addTrain()
{
    train tr;
    std::cin >> tr;
    std::cout << addTrain(tr) << std::endl;
}

void saleTrain()
{
    string id;
    train T;
    std::cin >> id;
    std::cout << saleTrain(id) << std::endl;

}

void queryTrain()
{
    string id;
    train T;
    std::cin >> id;
    if(!queryTrain(id, T))
    {
        std::cout << "0" << std::endl;
        return;
    }
    std::cout << T;
}

void deleteTrain()
{
    string id;
    train T;
    std::cin >> id;
    std::cout << deleteTrain(id) << std::endl;
}

void modifyTrain()
{
    train T;
    std::cin >> T;
    std::cout << modifyTrain(T) << std::endl;
}

void clean()
{
    idUser.clean();
    sale.clean();
    nSale.clean();
    locTrain.clean();
    direct.clean();
    transfer.clean();
    trKindTicket.clean();
    trCatTicket.clean();
    idTicket.clean();
    infoOrderId.clean();
    orderIdTicket.clean();
}

void init()
{
    idUser.open_file();
    sale.open_file();
    nSale.open_file();
    locTrain.open_file();
    direct.open_file();
    transfer.open_file();
    trKindTicket.open_file();
    trCatTicket.open_file();
    idTicket.open_file();
    infoOrderId.open_file();
    orderIdTicket.open_file();
}

int main()
{
    freopen("test.in", "r", stdin);
    const int funcNum = 17;
    string comm;
    string opt[funcNum];
    opt[0] = string("register");
    opt[1] = string("query_profile");
    opt[2] = string("modify_profile");
    opt[3] = string("modify_privilege");
    opt[4] = string("query_ticket");
    opt[5] = string("query_transfer");
    opt[6] = string("buy_ticket");
    opt[7] = string("query_order");
    opt[8] = string("refund_ticket");
    opt[9] = string("add_train");
    opt[10] = string("sale_train");
    opt[11] = string("query_train");
    opt[12] = string("delete_train");
    opt[13] = string("modify_train");
    opt[14] = string("clean");
    opt[15] = string("login");
    opt[16] = string("exit");
    void (*func[funcNum])() = {Register, queryProfile, modifyProfile, modifyPrivilege, queryTicket, queryTransfer, buyTicket,
                          queryOrder, refundTicket, addTrain, saleTrain, queryTrain, deleteTrain, modifyTrain, clean, login};
    init();
    int tot = 0;
    while(tot <= 100000)
    {
        tot++;
        bool flag = false;
        std::cin >> comm;
        if(comm == opt[16]) {
            std::cout << "BYE" << std::endl;
            break;
        }
        for(int i = 0; i < funcNum - 1; i++)
            if(comm == opt[i]) {
                func[i]();
                flag = true;
                break;
            }
        if(!flag) {
            std::cout << "WRONG COMMAND" << std::endl;
        }
    }
    clean();
    return 0;
}
