#ifndef MARKETITERATOR_H_
#define MARKETITERATOR_H_
#include <stdlib.h>
#include <string>
#include "datetime.h"
#include "customiterator.h"
class MarketIterator:public CustomIterator{
    public:
        double oraclePrice;
        double indexPrice;
        double fundingRate;
        double openInterest;
        MarketIterator(std::string coin, std::string exchange, std::string path);
        void iterate(Datetime d);
        void beginDay(std::string date);
        void endDay(std::string date);

};


#endif