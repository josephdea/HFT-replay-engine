#ifndef TRADESIGNAL_H_
#define TRADESIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class TradeSignal:public Signal{
    
    
    public:
        Orderbook &book;
        std::deque<Trade>RecentTrades;
        std::unordered_map<double,int>horizon_idx;
        TradeSignal(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Trade t);
        void processParams(signalconfig sc);
        void clear();
};
#endif