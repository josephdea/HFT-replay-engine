#ifndef CUTINSIGNAL_H_
#define CUTINSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class CutinSignal:public Signal{
    
    
    public:
        std::deque<std::pair<Datetime,CutinOrder>>CutinOrders;
        std::unordered_map<double,int>horizon_idx;
        Orderbook &book;
        CutinSignal(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Datetime d,CutinOrder c);
        void clear();
};
#endif