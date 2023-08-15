#include "cancelsignalmid.h"

CancelSignalMid::CancelSignalMid(std::string coin, std::string exchange, Nexus &n):Signal(n),
                                                                            book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"orderbook");
    nexus.allBookIterators[id]->subscribeCancelChange(boost::bind(&CancelSignalMid::update,this,boost::placeholders::_1,boost::placeholders::_2));
}

void CancelSignalMid::Preprocess(){
    
    int startidx = binary_search_upper<CancelOrder>(CancelledOrders,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,CancelledOrders.size()-1);
    for(int i = 0;i<startidx;i++){
        CancelledOrders.pop_front();
    }
    // if(coin == "BTC-USD" && exchange == "BNB"){
    //     std::cout << "cancel debug" << " " << CancelledOrders.size() << std::endl;
    // }
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
void CancelSignalMid::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    //std::vector<double>pllvect(paramsList.size(),0);
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
            double cs,rs,lc,horizon;
            //smode = std::stod(params["size_mode"]);
            horizon = std::stod(params["horizon"]);
            int j = std::stoi(params["write_idx"]);
            lc = std::stod(params["levelcap"]);
            cs = std::stod(params["c_size"]);
            rs = std::stod(params["r_size"]);
            int endidx = CancelledOrders.size()-1;
            int startidx = horizon_idx[horizon];
            double volume = 0;
            double numerator = 0;
            for(int i = startidx;i<endidx;i++){
                //double side = CancelledOrders[i].second.Side;
                double price = CancelledOrders[i].second.Price;
                int lvl = CancelledOrders[i].second.Level;
                //double price_diff = std::abs(book.Mid() - price);
                double rsize = CancelledOrders[i].second.RemainingSize;
                double csize = CancelledOrders[i].second.RemoveSize;
                if((lvl >= 2 && lvl <= lc) || (lvl == 1 && lc == 1)){
                    double effective_volume = csize * std::exp(-csize * cs) * std::exp(-rsize * rs);
                    numerator += effective_volume * price;
                    volume += effective_volume;
                }
            }
            double res;
            res = ((numerator/volume) - book.Mid()) / book.Mid();
            if(isnan(res) || isinf(res)){
                res = 0;
            }
            w[writeIdxOffset+j] = res;
            }   
    );
    
    //w.insert(w.end(),pllvect.begin(),pllvect.end());
}

void CancelSignalMid::update(Datetime d,CancelOrder c){
    // if(coin == "BTC-USD" && exchange == "BNB"){
    //     std::cout << d.string() << " " << c.Price << " " << c.Side << " " << c.Level << " " << c.RemoveSize << std::endl;
    // }
    CancelledOrders.push_back(std::make_pair(d,c));
}
void CancelSignalMid::clear(){
    CancelledOrders.clear();
}