#ifndef ORDEX_H_
#define ORDEX_H_
#include "nexus.h"
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>
#include <map>
class LimitOrder{
    public:
        double price;
        double size;
        double side;
        double sizeahead;
        LimitOrder();
        LimitOrder(double price, double size, double side, double sizeahead);
};


class Ordex{
    
    
    double takefee;
    double makefee;
    public:
        Nexus &nexus;
        Orderbook &book;
        std::string coin,exchange;
        std::string ordexname;
        std::vector<std::string>paramNames;
        int verbose;
        double position;
        double capital;
        double initialcapital;
        double volume;
        double minpnl;
        int zerocrosses;
        int oid;
        //std::unordered_map<std::string,std::string>paramsList;
        //std::deque<std::pair<Datetime,int>>positionDumper; //time/oid
        //std::unordered_map<int,Datetime>positionDumper;
        //BookIterator::BookIterator(std::string coin, std::string exchange, std::string path,Orderbook &book):CustomIterator(coin,exchange,path,"orderbook"),book(book){

        Ordex(std::string coin, std::string exchange,std::string path, Nexus &n);
        std::map<int,LimitOrder,std::greater<int>>ActiveBuyOrders;
        std::map<int,LimitOrder>ActiveSellOrders; //maps oids to limit order information
        int SendALO(double price, double size, double side);
        void SendIOC(double price, double size, double side);
        void Cancel1Order(int oid);
        void CancelAllOrders();
        double AvailableAssets();
        void printStatus();
        void update(Trade t);
        void subscribeCancel(Datetime d,CancelOrder c);
        //void onQuery(double prediction);
        void iterate(Datetime d);
        //void onQuery(Datetime d, double prediction);
        void beginDay();
        void endDay();
        void liquidatePosition();
        double getPNL();
        // std::string getColName();

};


#endif