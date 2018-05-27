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
#include "set.hpp"

typedef sjtu::string wchar;

/*
 * idUser      < id, user >

    Sale        < trainId, train >	sale
    nSale       < trainId, train >	notSale

    locTrain    < loc, trainSet >
    direct      < {loc1, loc2, catalog}, trainSet >	direct
    transfer    < {loc1, loc2, catalog}, trainPairSet >	transfer

    trKindTicket    < {trainId, date, kind}, ticketSet >
    trCatTicket     < {trainId, date, catalog}, ticketSet >
    idTicket        < {id, date, catalog}, ticketSet >
    infoOrderId     < {id, trainId, loc1, loc2, date, kind}, OrderId >
    OrderIdTicket   < OrderId, ticket >
 */

BPlusTree <int, user> idUser("user");
BPlusTree <wchar, train> Sale("sale");
BPlusTree <wchar, train> nSale("nsale");
BPlusTree <wchar, sjtu::set<train>> locTrain("locTrain");
BPlusTree <tuple<wchar, wchar, char>, sjtu::set<train>> direct("direct");
BPlusTree <tuple<wchar, wchar, char>, sjtu::set<train>> transfer("transfer");
BPlusTree <tuple<wchar, wchar, wchar>, sjtu::set<ticket>> trKindTicket("trKindTicket");
BPlusTree <tuple<wchar, wchar, wchar>, sjtu::set<ticket>> trCatTicket("trCatTicket");
BPlusTree <tuple<int, wchar, char>, sjtu::set<ticket>> idTicket("idTicket");
BPlusTree <tuple<int, wchar, wchar, wchar, wchar, wchar>, int> infoOrderId("infoOrderId");
BPlusTree <int, ticket> OrderIdTicket("OrderIdTicket");

int Register(wchar const &name, wchar const &pwd, wchar const &email, wchar const &phone);

bool queryProfile(int const &id, user &U);

void modifyProfile(int const &id, wchar const &name, wchar const &pwd, wchar const &email, wchar const &phone);

void modifyPrivilege(int const &id, int const &priv);

void queryTicket(pair<wchar, wchar> const &key, wchar const &date, wchar const &catalog);

void queryTransfer(pair<wchar, wchar> const &key, wchar const &date, wchar const &catalog);

int getTicketNum(wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind);

void buyTicket(int const &id, int const &num, wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind);

void queryOrderById(int const &id, wchar const &date, wchar const &catalog);

int getOrder(wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind);

void refundTicket(int const &id, int const &num, wchar const &trainId, wchar const &loc1, wchar const &loc2, wchar const &date, wchar const &kind);

bool queryAllTrain(wchar const &id);

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
void addTrain(wchar const &id);

void saleTrain(wchar const &id);

//id号火车是否public
bool queryTrain(wchar const &id, train &T);

void deleteTrain(wchar const &id);

void modifyTrain(wchar const &id);

void Register()
{
    wchar name, pwd, email, phone;
    std::cin >> name >> pwd >> email >> phone;
    std::cout << Register(name, pwd, email, phone) << std::endl;
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
    wchar name, pwd, email, phone;
    std::cin >> id >> name >> pwd >> email >> phone;
    if(!queryProfile(id, u))
        std::cout << "0" << std::endl;
    else
    {
        modifyProfile(id, name, pwd, email, phone);
        std::cout << "1" << std::endl;
    }
}

void modifyPrivilege()
{
    int id1, id2, priv;
    user u1, u2;
    std::cin >> id1 >> id2 >> priv;
    if(!queryProfile(id1, u1) || !queryProfile(id2, u2) || u1.pri() !=2 || u2.pri() > priv)
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
    queryTicket(pair<wchar, wchar>(loc1, loc2), date, catalog);
}

void queryTransfer()
{
    wchar loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    queryTransfer(pair<wchar, wchar>(loc1, loc2), date, catalog);
}

void buyTicket()
{
    // buy_ticket *id* *num* *train_id* *loc1* *loc2* *date* *ticket_kind*
    wchar trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    if(getTicketNum(trainId, loc1, loc2, date, kind) < num)
    {
        std::cout << "0" << std::endl;
        return;
    }
    buyTicket(id, num, trainId, loc1, loc2, date, kind);
    std::cout << "1" << std::endl;
}

void queryOrder()
{
    //query_order *id* *date* *catalog*
    int id;
    wchar date, catalog;
    user u;
    std::cin >> id >> date >> catalog;
    if(!queryProfile(id, u)) {
        std::cout << "-1" << std::endl;
        return;
    }
    queryOrderById(id, date, catalog);
}

void refundTicket()
{
    wchar trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    if(getOrder(trainId, loc1, loc2, date, kind) < num)
    {
        std::cout << "0" << std::endl;
        return;
    }
    refundTicket(id, num, trainId, loc1, loc2, date, kind);
    std::cout << "1" << std::endl;
}

void addTrain()
{
    wchar id;
    std::cin >> id;
    if(queryAllTrain(id))
    {
        std::cout << "0" << std::endl;
        return;
    }
    addTrain(id);
    std::cout << "1" << std::endl;
}

void saleTrain()
{
    wchar id;
    train T;
    std::cin >> id;
    if(queryTrain(id, T))
    {
        std::cout << "0" << std::endl;
        return;
    }
    saleTrain(id);
    std::cout << "1" << std::endl;
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
    std::cout << T;//TODO
}

void deleteTrain()
{
    wchar id;
    train T;
    std::cin >> id;
    if(queryTrain(id, T))
    {
        std::cout << "0" << std::endl;
        return;
    }
    deleteTrain(id);
    std::cout << "1" << std::endl;
}

void modifyTrain()
{
    wchar id;
    train T;
    std::cin >> id;
    if(!queryTrain(id, T))
    {
        std::cout << "0" << std::endl;
        return;
    }
    modifyTrain(id);
    std::cout << "1" << std::endl;
}

void clean();

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
