#ifndef CUSTOMITERATOR_H_
#define CUSTOMITERATOR_H_

#include <string>
#include <vector>
#include <unordered_map>
#include "datetime.h"
#include <cmath>
#include <ranges>
#include "float.h"
#include <math.h>
#include "fileparser.h"

class CustomIterator{
    public:
        FileParser fp;
        std::string coin;
        std::string exchange;
        int ctr;
        CustomIterator(std::string coin, std::string exchange);
        CustomIterator(std::string coin, std::string exchange, std::string path, std::string queryType);
        virtual Datetime peakTime();
        virtual void iterate(Datetime d)=0;
        virtual void onQuery(Datetime d){}
        virtual void beginDay(std::string date)=0;
        virtual void endDay(std::string date)=0;
        virtual void ClearFunctions(){}
};


#endif
