#ifndef GEOMETRICSIGNAL_H_
#define GEOMETRICSIGNAL_H_
#include "signal.h"
#include "nexus.h"
#include <algorithm>
#include <boost/fusion/container/vector.hpp>
class GeometricSignal:public Signal{
    //boost::fusion::container::vectors
    //std::unordered_map<std::vector<std::variant<std::string,double>>,int>um;
    int spreadIdx;
    std::unordered_map<int,int>paramToIncoming;
    public:
        GeometricSignal(Nexus &n);
        void Preprocess();
        void processParams(signalconfig sc);
        void ComputeSignal();
        //std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
        void clear();
};
#endif