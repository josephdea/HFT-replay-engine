#ifndef SIGNAL_H_
#define SIGNAL_H_
#include "nexus.h"
#include "configparser.h"
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>
#include<execution>
#include <boost/functional/hash.hpp>

class Signal{
    //gridsearch mode, how to handle parameter specifications
    public:
        Nexus &nexus;
        std::string coin;
        std::string exchange;
        bool gridsearchFlag;
        std::string SignalName;
        std::vector<std::string>paramNames;
        Datetime queryTime;
        int writeIdxOffset;
        std::vector<double>uniqueHorizons;
        //std::map<std::string,std::vector<double>>paramsGrid;
        std::vector<std::unordered_map<std::string,std::string>>paramsList;

        Signal(Nexus &n);
        std::vector<std::variant<std::string,double>> signalIdentifier(int idx);
        virtual void ComputeSignal()=0;
        virtual void Preprocess(){}
        //virtual std::vector<double> Compute(std::unordered_map<std::string,std::string>&params)=0;
        virtual std::vector<std::string> getColNames();
        virtual void processParams(signalconfig sc);
        virtual void clear()=0;
        //virtual void update(Datetime,CancelOrder);
};
bool isFloat( std::string myString );

std::vector<std::variant<std::string,double>>generatesignalId(std::string);
//find first time bigger than given time d
template<typename T> int binary_search_upper(std::deque<std::pair<Datetime,T>>&container,Datetime d,int left, int right){
    int res = right+1;
    while(left <= right){
        int mid = (left+right)/2;

        if(container[mid].first > d){
            res = mid;
            right = mid-1;
            
        }
        else{
            left = mid + 1;
            
        }
    }
    return res;
}
#endif