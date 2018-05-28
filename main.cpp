//
// Created by Arya on 2018/5/11.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include "utility.hpp"
#include "user.hpp"
#include "train.hpp"
#include "ticket.hpp"
#include "BPlusTree.h"
#include "tuple.hpp"
#include "map.hpp"
#include "queue.hpp"
#include "constant.h"

typedef sjtu::string wchar;
typedef sjtu::train train;
typedef sjtu::ticket ticket;
typedef sjtu::map <wchar, train> trainSet;
typedef sjtu::map <wchar, ticket> ticketSet;
typedef sjtu::user user;
typedef sjtu::time Time;

const int CAPACITY = sjtu::TRAIN_CAPACITY ;

/*
 * idUser      < id, user >

    sale        < trainId, train >	sale
    nSale       < trainId, train >	notSale

    locTrain    < loc, trainSet >
    direct      < {loc1, loc2, catalog}, trainSet >	direct
    transfer    < {loc1, loc2, catalog}, trainPairSet >	transfer

    trKindTicket    < {trainId, date, kind}, ticketSet >
    trCatTicket     < {trainId, date, catalog}, ticketSet >
    idTicket        < {id, date, catalog}, ticketSet >
    infoOrderId     < {id, trainId, loc1, loc2, date, kind}, OrderId >
    orderIdTicket   < OrderId, ticket >
 */

//TODO:cmp
sjtu::BPlusTree <int, user> idUser("user");
sjtu::BPlusTree <wchar, train> sale("sale");
sjtu::BPlusTree <wchar, train> nSale("nsale");
sjtu::BPlusTree <wchar, sjtu::map<wchar, train> > locTrain("locTrain");
sjtu::BPlusTree <sjtu::tuple<wchar, wchar, char>, trainSet> direct("direct");
sjtu::BPlusTree <sjtu::tuple<wchar, wchar, char>, sjtu::tuple<Time, train, train> > transfer("transfer");
sjtu::BPlusTree <sjtu::tuple<wchar, wchar, wchar>, ticketSet> trKindTicket("trKindTicket");
sjtu::BPlusTree <sjtu::tuple<wchar, wchar, wchar>, ticketSet> trCatTicket("trCatTicket");
sjtu::BPlusTree <sjtu::tuple<int, wchar, char>, ticketSet> idTicket("idTicket");
sjtu::BPlusTree <sjtu::tuple<int, wchar, wchar, wchar, wchar, wchar>, int> infoOrderId("infoOrderId");
sjtu::BPlusTree <int, ticket> orderIdTicket("orderIdTicket");

bool intersect(ticket const &tic, wchar const &loc1, wchar const &loc2)
{
    //TODO
}

int Register(user &u)
{
    int id = ++sjtu::user::idCnt;
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
    user now(pre);
    now.priv = priv;
    idUser.modify(id, now);
}

void queryTicket(wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &catalog)
{
    typedef sjtu::map <wchar, int> map;
    int len = catalog.length();
    sjtu::queue <sjtu::pair<train, sjtu::map <wchar, int> > > que;
    for(int i = 0; i < len; ++i) {
        char cat = catalog[i];
        auto allTrain = direct.find(sjtu::tuple<wchar, wchar, char>(loc1, loc2, cat)).second;
        for(auto it = allTrain.begin(); it != allTrain.end(); ++it) {   //guarantees the order of trains
            train tr = it->second;
            int sold = 0;
            sjtu::map <wchar, int> used;
            auto allTicket = trCatTicket.find(sjtu::tuple<wchar, wchar, char>(tr.getId(), date, cat)).second;
            for(auto j = allTicket.begin(); j != allTicket.end(); ++j) {
                ticket tic = j->second;
                if(intersect(tic, loc1, loc2)) {
                    if(used[tic.getKind()] += tic.getNum() == CAPACITY)
                        sold++;
                }
            }
            if(sold < tr.priceNum) {
                que.push(sjtu::pair<train, sjtu::map <wchar, int> >(tr, used));
            }
        }
    }
    std::cout << que.size() << std::endl;
    while(!que.empty()) {
        auto tr = que.front().first;
        auto used = que.front().second;
        std::cout << tr.id << ' ' << loc1 << ' ' << sjtu:::getTime(tr, date, loc1) << loc2 << sjtu::getTime(tr, date, loc2);
        for(int i = 0; i < tr.priceNum; ++i) {
            int usedTic = used[tr.priceName[i]];
            if(usedTic < CAPACITY) {
                std::cout << ' ' << tr.priceName[i] << ' ' << CAPACITY - usedTic << ' ' << sjtu::getPrice(tr, loc1, loc2, i);
            }
            std::cout << std::endl;
        }
    }
}

void queryTransfer(wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &catalog)
{
    //TODO
}

void insertOrder(int const &id, int const &num, wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind)
{
    auto key = sjtu::tuple<int, wchar, wchar, wchar, wchar, wchar>(id, trainId, loc1, loc2, date, kind);
    auto tmp = infoOrderId.find(key);
    auto orderId = tmp.second;
    if(!tmp.first) {
        //create a new order
        int cnt = ++ticket::orderCnt;
        auto tr = sale.find(trainId).second;
        char catalog = tr.catalog;
        ticket newTic(id, cnt, num, trainId, loc1, loc2, date, kind, catalog);
        auto toInsert = sjtu::pair<wchar, ticket>(trainId ,newTic); //TODO: ticketSet: orderId or ticket ??
        orderIdTicket.insert(cnt, newTic);

        auto ticSet1 = trKindTicket.find(sjtu::tuple<wchar, wchar, wchar>(trainId, date, kind)).second;
        ticSet1.insert(toInsert);
        trKindTicket.modify(sjtu::tuple<wchar, wchar, wchar>(trainId, date, kind), ticSet1);

        auto ticSet2 = trCatTicket.find(sjtu::tuple<wchar, wchar, char>(trainId, date, catalog)).second;
        ticSet2.insert(toInsert);
        trCatTicket.modify(sjtu::tuple<wchar, wchar, char>(trainId, date, catalog), ticSet2);

        auto ticSet3 = idTicket.find(sjtu::tuple<int, wchar, char>(id, date, catalog)).second;
        ticSet3.insert(toInsert);
        idTicket.modify(sjtu::tuple<int, wchar, char>(id, date, catalog), ticSet3);

        infoOrderId.insert(sjtu::tuple<int, wchar, wchar, wchar, wchar, wchar>(id, trainId, loc1, loc2, date, kind), orderId);
    }
    else {
        auto tic = orderIdTicket.find(orderId).second;
        tic.modifyNum(num);
        orderIdTicket.modify(orderId, tic);
    }
}

bool buyTicket(int const &id, int const &num, wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind) {
    int used = 0;
    auto allTicket = trKindTicket.find(sjtu::tuple<wchar, wchar, wchar>(trainId, date, kind)).second;
    for(auto it = allTicket.begin(); it != allTicket.end(); ++it) {
        auto ticket = it->second;
        if(intersect(ticket, loc1, loc2)) {
            used += ticket.getNum();
        }
    }
    if(used < CAPACITY) {
        insertOrder(id, num, trainId, loc1, loc2, date, kind);
        return true;
    }
    return false;
}

bool queryOrder(int const &id, wchar const &date, char const &catalog)
{
    auto tmp = idTicket.find(sjtu::tuple<int, wchar, char>(id, date, catalog));
    if(!tmp.first)
        return false;
    auto ticketSet = tmp.second;
    std::cout << ticketSet.size() << std::endl;
    for(auto it = ticketSet.begin(); it != ticketSet.end(); ++it) {
        auto tic = it->second;
        auto tr = sale.find(tic.trainId).second;
        std::cout << tic.trainId << ' ';
        std::cout << tic.loc1 << ' ' << sjtu::getTime(tr, date, tic.loc1) << ' ';
        std::cout << tic.loc2 << ' ' << sjtu::getTime(tr, date, tic.loc2) << ' ';
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
    return true;
}


bool refundTicket(int const &id, int const &num, wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind)
{
    auto tmp = infoOrderId.find(sjtu::tuple<int, wchar, wchar, wchar, wchar, wchar>(id, trainId, loc1, loc2, date, kind));
    if(!tmp.first)
        return false;
    auto orderId = tmp.second;
    auto tic = orderIdTicket.find(orderId).second;
    if(tic.num < num)
        return false;
    tic.modifyNum(-num);
    orderIdTicket.modify(orderId, tic);
    return true;
}

bool addTrain(train const &tr)
{
    if(sale.find(tr.id).first || nSale.find(tr.id).first)
        return false;
    nSale.insert(tr.id, tr);
    return true;
}

Time calTime(train const &tr1, train const &tr2, wchar const &loc0, wchar const &loc1, wchar const &loc2)
{

}

bool saleTrain(wchar const &id)
{
    auto tmp0 = nSale.find(id);
    auto tr = tmp0.second;
    if(!tmp0.first)
        return false;
    nSale.erase(id);
    sale.insert(id, tr);

    char catalog = tr.catalog;
    for(int i = 0; i < tr.stationNum; ++i) {
        wchar loc1 = tr.stationName[i];
        for(int j = i + 1; j < tr.stationNum; ++j) {
            wchar loc2 = tr.stationName[j];
            auto key = sjtu::tuple<wchar, wchar, char>(loc1, loc2, catalog);
            auto tmp = direct.find(key);
            auto trainS = tmp.second;
            trainS.insert(sjtu::pair<wchar, train>(id, tr));
            if(tmp.first) {
                direct.modify(key, trainS);
            }
            else {
                direct.insert(key, trainS);
            }
        }
    }

    for(int i = 0; i < tr.stationNum; ++i) {
        wchar loc1 = tr.stationName[i];
        auto trainS = locTrain.find(loc1).second;
        for(auto it = trainS.begin(); it != trainS.end(); ++it) {
            auto tr0 = it->second;
            for(int k = 0; k < tr0.stationNum; ++k) {
                wchar loc0 = tr0.stationName[k];
                if(loc0 == loc1)  break;
                for(int j = i + 1; j < tr.stationNum; ++j) {
                    wchar loc2 = tr.stationName[j];
                    auto key = sjtu::tuple<wchar, wchar, char>(loc0, loc2, catalog);
                    auto tmp = transfer.find(key);
                    if(!tmp.first) {
                        transfer.insert(key, sjtu::tuple<Time, train, train>(calTime(tr0, tr, loc0, loc1, loc2), tr0, tr));
                    }
                    else {
                        auto minTime = tmp.second.head();
                        auto now = calTime(tr0, tr, loc0, loc1, loc2);
                        if(now < minTime) {
                            transfer.modify(key, sjtu::tuple<Time, train, train>(now, tr0, tr));
                        }
                    }
                }
            }
        }
    }

    for(int i = 0; i < tr.stationNum; ++i) {
        wchar loc1 = tr.stationName[i];
            for(int j = i + 1; j < tr.stationNum; ++j) {
                wchar loc2 = tr.stationName[j];
                auto trainS = locTrain.find(loc2).second;
                for(auto it = trainS.begin(); it != trainS.end(); ++it) {
                    auto tr1 = it->second;
                    for(int k = tr1.stationNum; k > 0; --k) {
                        wchar loc3 = tr1.stationName[k];
                        if(loc3 == loc1)  break;
                        auto key = sjtu::tuple<wchar, wchar, char>(loc1, loc3, catalog);
                        auto tmp = transfer.find(key);
                        if(!tmp.first) {
                            transfer.insert(key, sjtu::tuple<Time, train, train>(calTime(tr, tr1, loc1, loc2, loc3), tr, tr1));
                        }
                        else {
                            auto minTime = tmp.second.head();
                            auto now = calTime(tr, tr1, loc1, loc2, loc3);
                            if(now < minTime) {
                                transfer.modify(key, sjtu::tuple<Time, train, train>(now, tr, tr1));
                            }cm
                        }
                }
            }
    }
}

bool queryTrain(wchar const &id, train &T)
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

bool deleteTrain(wchar const &id)
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

void queryProfile()
{
    int id;
    user u;
    std::cin >> id;
    if(!queryProfile(id, u))
        std::cout << "0" << std::endl;
    else
        std::cout << u << std::endl; //TODO
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
    wchar loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    queryTicket(loc1, loc2, date, catalog);
}

void queryTransfer()
{
    wchar loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    queryTransfer(loc1, loc2, date, catalog);
}

void buyTicket()
{
    // buy_ticket *id* *num* *train_id* *loc1* *loc2* *date* *ticket_kind*
    wchar trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    std::cout << buyTicket(id, num, trainId, loc1, loc2, date, kind) << std::endl;
}

void queryOrder()
{
    //query_order *id* *date* *catalog*
    int id;
    wchar date;
    char catalog;
    user u;
    std::cin >> id >> date >> catalog;
    if(!queryOrder(id, date, catalog))
        std::cout << "-1" << std::endl;
}

void refundTicket()
{
    wchar trainId, loc1, loc2, date, kind;
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
    wchar id;
    train T;
    std::cin >> id;
    std::cout << saleTrain(id) << std::endl;

}

void queryTrain()
{
    wchar id;
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
    wchar id;
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

}

void init()
{

}

int main()
{
    wchar opt[16];
    wchar comm;
    opt[0] = wchar("register");
    opt[1] = wchar("query_profile");
    opt[2] = wchar("modify_profile");
    opt[3] = wchar("modify_privilege");
    opt[4] = wchar("query_ticket");
    opt[5] = wchar("query_transfer");
    opt[6] = wchar("buy_ticket");
    opt[7] = wchar("query_order");
    opt[8] = wchar("refund_ticket");
    opt[9] = wchar("add_train");
    opt[10] = wchar("sale_train");
    opt[11] = wchar("query_train");
    opt[12] = wchar("delete_train");
    opt[13] = wchar("modify_train");
    opt[14] = wchar("clean");
    opt[15] = wchar("exit");
    void (*func[16])() = {Register, queryProfile, modifyProfile, modifyPrivilege, queryTicket, queryTransfer, buyTicket,
                          queryOrder, refundTicket, addTrain, saleTrain, queryTrain, deleteTrain, modifyTrain, clean};
    init();
    while(true)
    {
        std::cin >> comm;
        for(int i = 0; i < 15; i++)
            if(comm == opt[i]) {
                func[i];
                break;
            }
        if(comm == opt[15]) {
            std::cout << "BYE" << std::endl;
            break;
        }
    }
    return 0;
}
