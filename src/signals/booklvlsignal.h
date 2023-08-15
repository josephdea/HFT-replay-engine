#ifndef BOOKLVLSIGNAL_H_
#define BOOKLVLSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class BooklvlSignal:public Signal{
    
    
    public:
        Orderbook &book;
        BooklvlSignal(std::string coin, std::string exchange, Nexus &n);
        //void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        //void update(Datetime d,CancelOrder c);
        void clear();
};
#endif