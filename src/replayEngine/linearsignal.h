#ifndef LINEARSIGNAL_H_
#define LINEARSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>
class LinearSignal:public Signal{
    //boost::fusion::container::vectors
    //std::unordered_map<std::vector<std::variant<std::string,double>>,int>um;
    double intercept;
    std::vector<int>idxes;
    std::vector<double>coefficients;
    public:
        LinearSignal(Nexus &n);
        void processParams(signalconfig sc);
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void clear();
        std::vector<std::string> getColNames();
};
#endif