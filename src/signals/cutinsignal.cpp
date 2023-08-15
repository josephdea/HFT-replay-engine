#include "cutinsignal.h"
#include <math.h>

CutinSignal::CutinSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){

    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"orderbook");
    nexus.allBookIterators[id]->subscribeCutinChange(boost::bind(&CutinSignal::update,this,boost::placeholders::_1,boost::placeholders::_2));

}
void CutinSignal::Preprocess(){
    int startidx = binary_search_upper<CutinOrder>(CutinOrders,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,CutinOrders.size()-1);
    for(int i = 0;i<startidx;i++){
        CutinOrders.pop_front();
    }
    int left = 0;
    int right = CutinOrders.size()-1;
    for(int i = uniqueHorizons.size()-1;i>=0;i--){
        double h = uniqueHorizons[i];
        left = binary_search_upper<CutinOrder>(CutinOrders,this->queryTime - h,left,right);
        horizon_idx[h] = left;
    }
}
void CutinSignal::ComputeSignal(){
    int idx = nexus.features.size()-1;
    //std::vector<double>pllvect(paramsList.size(),0);
    std::vector<double>&w = nexus.features[idx];
    //if(paramsList.size()>= 10){
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
            double cs = std::stod(params["c_size"]);
            double cc = std::stod(params["c_cutin"]);
            double norm = std::stod(params["norm"]);
            double horizon = std::stod(params["horizon"]);
            int j = std::stoi(params["write_idx"]);
            double askStrength = 0;
            double bidStrength = 0;
            int endidx = CutinOrders.size()-1;
            int startidx = horizon_idx[horizon];

            for(int i = startidx;i<endidx;i++){
                int side = CutinOrders[i].second.Side;
                double spread = CutinOrders[i].second.Spread;
                double effective_sz = pow(CutinOrders[i].second.Size,cs);
                double cutin_amount = CutinOrders[i].second.CutinAmount;
                if(side == 1){
                    askStrength += effective_sz * exp(-cc * cutin_amount);
                }
                else{
                    bidStrength += effective_sz * exp(-cc * cutin_amount);
                }
            }
            double res;
            if(norm == 0){
                res = log(askStrength + 1e-8) - log(bidStrength + 1e-8);
            }
            else if(norm == 1){
                res = (askStrength - bidStrength) / (askStrength + bidStrength+1e-8);
            }
            else{
                res = askStrength - bidStrength;
            }
            // if (isnan(res)){
            //     std::cout << askStrength << " " <<bidStrength << std::endl;
            //     exit(1);
            // }
            if(isnan(res) || isinf(res)){
                res = 0;
            }
            w[writeIdxOffset+j] = res;
            }   
    );
    
    //w.insert(w.end(),pllvect.begin(),pllvect.end());
}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
void CutinSignal::update(Datetime d,CutinOrder c){
    CutinOrders.push_back(std::make_pair(d,c));
}
void CutinSignal::clear(){
    CutinOrders.clear();
}