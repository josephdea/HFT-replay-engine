#include "linearsignal.h"
LinearSignal::LinearSignal(Nexus &n):Signal(n){

}
void LinearSignal::processParams(signalconfig sc){
    this->intercept = std::stod(sc.params["Intercept"][0]);
    if(sc.params["Coefficients"].size() != sc.params["Signals"].size()){
        std::cout << "linear regression coefficient and signal msimatch size" << std::endl;
        exit(1);
    }
    for(int i = 0;i<sc.params["Signals"].size();i++){
        auto sid = generatesignalId(sc.params["Signals"][i]);
        if(nexus.signalidx.find(sid) == nexus.signalidx.end()){
            std::cout <<"ERROR: linear regression signal idx not found " << std::endl;
            exit(1);
        }
        idxes.push_back(nexus.signalidx[sid]);
        coefficients.push_back(std::stod(sc.params["Coefficients"][i]));
    }
}

void LinearSignal::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    double running_sum = this->intercept;
    for(int i = 0;i<coefficients.size();i++){
        running_sum += coefficients[i] * w[idxes[i]];
    }
    w[writeIdxOffset] = running_sum;
}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
void LinearSignal::clear(){

}
std::vector<std::string> LinearSignal::getColNames(){
    return std::vector<std::string>{"LinearRegression"};
}