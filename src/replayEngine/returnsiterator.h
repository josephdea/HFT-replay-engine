#ifndef RETURNSITERATOR_H_
#define RETURNSITERATOR_H_
#include "customiterator.h"
#include "orderbook.h"

class ReturnsIterator:public CustomIterator{
    public:
        Orderbook &book;
        std::vector<double>horizons;
        std::vector<std::tuple<Datetime,int,double,double>>QueryHeap; //time,index,bestbid,bestask
        std::vector<std::vector<double>>toWrite;
        ReturnsIterator(std::string coin, std::string exchange,Orderbook &book,std::vector<double>horizons);
        Datetime peakTime();
        void iterate(Datetime d);
        void beginDay(std::string date);
        void onQuery(Datetime d);
        void endDay(std::string date);
        std::vector<std::string> getColNames();
};
#endif