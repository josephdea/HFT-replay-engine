#include "maker.h"


Maker::Maker(std::string coin, std::string exchange,std::string path, Nexus &n):Strategy(coin,exchange,path,n){

}
void Maker::onQuery(Datetime d, double prediction){
    
    
    double spread;
    //double abs_prediction = abs(prediction) * 1000;
    // if(spread_mode == 0){ //test using spread from effective size
    int idx = ordex.nexus.features.size()-1;
    std::vector<double>&w = ordex.nexus.features[idx];
    spread =  0.0001 * ordex.book.Mid() * spread_scaler + 0.4 * w[spreadidx] * vol_scaler; //pred_scaler * abs_prediction;
    
    // else if(spread_mode == 1){ //test using bps of the current mid
    //     spread =  0.0001 * ordex.book.Mid() * spread_scaler;
    // }
    if(ordex.verbose){
        std::cout << "quoted spread: " << spread << std::endl;
    }
    //mess around with function for biasing mid price
    double newmid = (1+prediction) * ordex.book.Mid() - mid_bias * ordex.position;
    if(ordex.verbose && ordex.position != 0){
        std::cout << "bias mid price by: " << mid_bias * ordex.position << std::endl;
    }
    double askpx = std::ceil(newmid + 0.5 * spread);
    double bidpx = std::floor(newmid - 0.5 * spread);
    askpx = std::max(ordex.book.BestAsk(),askpx);
    bidpx = std::min(ordex.book.BestBid(),bidpx);
    double asksz = size;
    double bidsz = size;
    std::vector<int>clearorders;
    for(const auto &[key,value]:ordex.ActiveSellOrders){
        if(std::abs(value.price - askpx) > adjust_threshold){
            clearorders.push_back(key);
        }
        else{
            asksz -= value.size;
        }
        
    }
    for(auto it:clearorders){
        ordex.Cancel1Order(it);
    }
    clearorders.clear();
    for(const auto &[key,value]:ordex.ActiveBuyOrders){
        if(std::abs(value.price - bidpx) > adjust_threshold){ //previous quoted price too far away from the price i want
            clearorders.push_back(key);
        }
        else{                                       //let the order rest
            bidsz -= value.size;              
        }
    }

    for(auto it:clearorders){
        ordex.Cancel1Order(it);
    }
    if(asksz < 0){
        std::cout <<"debug ask size" << std::endl;
        std::cout << asksz << std::endl;
    }
    if(bidsz < 0){
        std::cout <<"debug bid size" << std::endl;
        std::cout << bidsz << std::endl;
    }
    if(limit_size == 1){
        if(ordex.position > 0){
            bidsz -= ordex.position;
        }else{
            asksz += ordex.position;
        }
    }
    
    //mess around with placing orders at levels that dont have any1 on them
    //place orders everywhere and cancel if fair price isnt where i want it
    if(asksz < -0.00001 || bidsz < -0.00001){
        std::cout << "error with ask sz or bid sz" << std::endl;
        std::cout << asksz << " " << bidsz << std::endl;
    }
    if(asksz > 0)ordex.SendALO(askpx,asksz,1);
    if(bidsz > 0)ordex.SendALO(bidpx,bidsz,-1);

}
void Maker::beginDay(){
    mid_bias = std::stod(paramsList["mid_bias"]);
    size = std::stod(paramsList["size"]);
    adjust_threshold = std::stod(paramsList["adjust_threshold"]);
    spread_scaler = std::stod(paramsList["spread_scaler"]);
    vol_scaler = std::stod(paramsList["vol_scaler"]);
    limit_size = std::stod(paramsList["limit_size"]);
    //pred_scaler = std::stod(paramsList["pred_scaler"]);

    for(const auto &[key,value]:ordex.nexus.signalidx){
        if(std::get<std::string>(key[0]).compare("EffectiveSpread") == 0){
            spreadidx = value;
        }
    }
    ordex.beginDay();
    ordex.verbose = std::stoi(paramsList["verbose"]);
}
void Maker::endDay(){

}