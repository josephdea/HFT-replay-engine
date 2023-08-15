#include "ordex.h"


#ifndef STRATEGY_H_
#define STRATEGY_H_

class Strategy{
    public:
        std::string strategyName;
        std::vector<std::string>paramNames;
        std::unordered_map<std::string,std::string>paramsList;
        Ordex ordex;
        Strategy(std::string coin, std::string exchange,std::string path, Nexus &n);
        virtual void onQuery(Datetime d, double prediction)=0;
        virtual void beginDay()=0;
        virtual void endDay()=0;
        std::string getColName();
};


#endif 