#ifndef CANCELSIGNALMID_H_
#define CANCELSIGNALMID_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class CancelSignalMid:public Signal{
    
    
    public:
        std::deque<std::pair<Datetime,CancelOrder>>CancelledOrders;
        std::unordered_map<double,int>horizon_idx;
        Orderbook &book;
        CancelSignalMid(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Datetime d,CancelOrder c);
        void clear();
};
#endif