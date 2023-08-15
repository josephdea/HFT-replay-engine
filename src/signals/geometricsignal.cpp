#include "geometricsignal.h"

GeometricSignal::GeometricSignal(Nexus &n):Signal(n){

}
void debug1(std::vector<std::variant<std::string,double>> it){
    for(auto it1:it){
        std::visit([](auto&& arg){ std::cout << arg << " "; }, it1);
        //std::cout << it1 << std::endl;
    }
    std::cout << std::endl;
}
void GeometricSignal::processParams(signalconfig sc){
    this->SignalName = sc.signalname;
    this->paramNames = sc.colNamesOrdered;
    this->paramsList = combinations(sc.params);
    for(const auto &[key,value]:nexus.signalidx){
        if(std::get<std::string>(key[0]).compare("EffectiveSpread") == 0){
            spreadIdx = value;
        }
    }
    for(int i = 0;i<paramsList.size();i++){
        std::string incSignal = paramsList[i]["inc_signal"];
        auto sid = generatesignalId(incSignal);
        
        if(nexus.signalidx.find(sid) == nexus.signalidx.end()){
            std::cout <<"ERROR: signal idx not found " << std::endl;
            debug1(sid);
            exit(1);
        }
        //std::cout << incSignal << " index found " << nexus.signalidx[sid] << std::endl;
        paramToIncoming[i] = nexus.signalidx[sid];

    }
}
void GeometricSignal::Preprocess(){//determine location of effective spread and location of signal
    
}
void GeometricSignal::ComputeSignal(){
    //std::cout << "computing geometric signal " << std::endl;
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    for(int i = 0;i<paramsList.size();i++){
        double effective_spread = w[spreadIdx];
        double inc_signal_val = w[paramToIncoming[i]];
        double res;
        if(inc_signal_val < 0){
            res = -1 * std::pow(std::abs(inc_signal_val) * effective_spread,0.5);
        }
        else{
            res = std::pow(std::abs(inc_signal_val) * effective_spread,0.5);;
        }
        w[writeIdxOffset + i] = res;
    }

}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
void GeometricSignal::clear(){
    
}