#include "strategy.h"
#ifndef TAKER_H_
#define TAKER_H_

class Taker:public Strategy{
    public:
        std::unordered_map<int,Datetime>positionDumper;
        Taker(std::string coin, std::string exchange,std::string path, Nexus &n);
        void onQuery(Datetime d, double prediction);
        void beginDay();
        void endDay();
};

#endif