#ifndef BOOKPRINTSIGNAL_H_
#define BOOKPRINTSIGNAL_H_
#include "signal.h"
#include "nexus.h"
class BookPrintSignal:public Signal{
    public:
        Orderbook &book;
        BookPrintSignal(std::string coin, std::string exchange, Nexus &n);
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        std::vector<std::string> getColNames();
        void clear();
        //void update(Datetime d,CancelOrder c);
};
#endif