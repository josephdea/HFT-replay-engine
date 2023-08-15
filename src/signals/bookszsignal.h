#ifndef BOOKSZSIGNAL_H_
#define BOOKSZSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class BookszSignal:public Signal{
    
    
    public:
        Orderbook &book;
        BookszSignal(std::string coin, std::string exchange, Nexus &n);
        void ComputeSignal();
        void clear();
};
#endif