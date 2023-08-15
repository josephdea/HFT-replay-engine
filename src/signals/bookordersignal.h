#ifndef BOOKORDERSSIGNAL_H_
#define BOOKORDERSSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class BookordersSignal:public Signal{
    
    
    public:
        Orderbook &book;
        BookordersSignal(std::string coin, std::string exchange, Nexus &n);
        void ComputeSignal();
        void clear();
};
#endif