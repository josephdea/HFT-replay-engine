#ifndef CANCELSIGNALIMBA_H_
#define CANCELSIGNALIMBA_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class CancelSignalImba:public Signal{
    
    
    public:
        std::deque<std::pair<Datetime,CancelOrder>>CancelledOrders;
        std::unordered_map<double,int>horizon_idx;
        Orderbook &book;
        CancelSignalImba(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Datetime d,CancelOrder c);
        void clear();
};
#endif