#include "cancelsignalimba.h"

CancelSignalImba::CancelSignalImba(std::string coin, std::string exchange, Nexus &n):Signal(n),
                                                                            book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"orderbook");
    nexus.allBookIterators[id]->subscribeCancelChange(boost::bind(&CancelSignalImba::update,this,boost::placeholders::_1,boost::placeholders::_2));
}

void CancelSignalImba::Preprocess(){
    int startidx = binary_search_upper<CancelOrder>(CancelledOrders,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,CancelledOrders.size()-1);
    for(int i = 0;i<startidx;i++){
        CancelledOrders.pop_front();
    }
    int left = 0;
    int right = CancelledOrders.size()-1;
    for(int i = uniqueHorizons.size()-1;i>=0;i--){
        double h = uniqueHorizons[i];
        left = binary_search_upper<CancelOrder>(CancelledOrders,this->queryTime - h,left,right);
        // std::cout << left << " " << right << " " << h << std::endl;
        // std::cout << (this->queryTime-h).string() << std::endl;
        // std::cout << CancelledOrders[left].first.string() << std::endl;
        horizon_idx[h] = left;
    }
}
void CancelSignalImba::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    //std::vector<double>pllvect(paramsList.size(),0);
    auto params = paramsList[0];
    //for(auto params:paramsList){
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
            double cs,rs,lc,cl,norm,horizon;
            horizon = std::stod(params["horizon"]);
            int j = std::stoi(params["write_idx"]);
            lc = std::stod(params["levelcap"]);
            cs = std::stod(params["c_size"]);
            rs = std::stod(params["r_size"]);
            cl = std::stod(params["c_lvl"]);
            norm = std::stod(params["norm"]);

            double askStrength = 0;
            double bidStrength = 0;
            int endidx = CancelledOrders.size()-1;
            int startidx = horizon_idx[horizon];
            for(int i = startidx;i<endidx;i++){
                double side = CancelledOrders[i].second.Side;
                double price = CancelledOrders[i].second.Price;
                int lvl = CancelledOrders[i].second.Level;
                double price_diff = std::abs(book.Mid() - price);
                double rsize = CancelledOrders[i].second.RemainingSize;
                double csize = CancelledOrders[i].second.RemoveSize;
                if((lvl >= 2 && lvl <= lc) || (lvl == 1 && lc == 1)){
                    double value = std::pow(rsize,rs) * std::pow(csize,cs) *  std::exp(-cl * price_diff);//* std::exp(-ct * (timediff - offset));
                    if(side == 1){
                        askStrength += value;
                    }   
                    else{
                        bidStrength += value;
                    }
                }
            }
            double res;
            if(norm == 1){
            res = log(askStrength + 1e-8) - log(bidStrength + 1e-8);
            }
            else if(norm == 2){
                res = (askStrength - bidStrength) / (askStrength + bidStrength+1e-8);
            }
            else{
                res = askStrength - bidStrength;
            }
            if(isnan(res) || isinf(res)){
                res = 0;
            }
            w[writeIdxOffset+j] = res;
            }   
   

    );
    
    //w.insert(w.end(),pllvect.begin(),pllvect.end());
}

void CancelSignalImba::update(Datetime d,CancelOrder c){

    CancelledOrders.push_back(std::make_pair(d,c));
}
void CancelSignalImba::clear(){
    CancelledOrders.clear();
}