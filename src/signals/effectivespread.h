#ifndef EFFECTIVESPREADSIGNAL_H_
#define EFFECTIVESPREADSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>

class EffectiveSpread:public Signal{
    
    
    public:
        Orderbook &book;
        EffectiveSpread(std::string coin, std::string exchange, Nexus &n);
        //void ();
        void ComputeSignal();
        
        void clear();
};
#endif