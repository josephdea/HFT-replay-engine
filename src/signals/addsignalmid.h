#ifndef ADDSIGNALMID_H_
#define ADDSIGNALMID_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class AddSignalMid:public Signal{
    
    
    public:
        std::deque<std::pair<Datetime,AddOrder>>AddedOrders;
        std::unordered_map<double,int>horizon_idx;
        Orderbook &book;
        AddSignalMid(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Datetime d,AddOrder a);
        void clear();
};
#endif