#ifndef REVERSIONSIGNAL_H_
#define REVERSIONSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class ReversionSignal:public Signal{
    
    
    public:
        std::deque<std::pair<Datetime,double>>PrevMids;
        std::unordered_map<double,int>horizon_idx;
        std::unordered_map<double,int>trade_horizon_idx;
        std::deque<Trade>RecentTrades;
        Orderbook &book;
        ReversionSignal(std::string coin, std::string exchange, Nexus &n);
        void Preprocess();
        void ComputeSignal();
        void processParams(signalconfig sc);
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void update(Datetime d,double mid);
        void update(Trade t);
        void clear();
};
#endif