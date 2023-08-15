#include "addsignalimba.h"
//#include <functional>
//using namespace std::placeholders;
// #include <boost/array.hpp>

AddSignalImba::AddSignalImba(std::string coin, std::string exchange, Nexus &n):Signal(n),
                                                                            book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"orderbook");
    nexus.allBookIterators[id]->subscribeAddChange(boost::bind(&AddSignalImba::update,this,boost::placeholders::_1,boost::placeholders::_2));
}

void AddSignalImba::Preprocess(){
    int startidx = binary_search_upper<AddOrder>(AddedOrders,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,AddedOrders.size()-1);
    for(int i = 0;i<startidx;i++){
        AddedOrders.pop_front();
    }
    int left = 0;
    int right = AddedOrders.size()-1;
    for(int i = uniqueHorizons.size()-1;i>=0;i--){
        double h = uniqueHorizons[i];
        left = binary_search_upper<AddOrder>(AddedOrders,this->queryTime - h,left,right);
        horizon_idx[h] = left;
    }
}
void AddSignalImba::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    //if(paramsList.size()>= 10){
    double curmid = book.Mid();
    auto params = paramsList[0];
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
        //for(auto params:paramsList){
            double norm = std::stod(params["norm"]);
            double cl = std::stod(params["c_lvl"]);
            double cs = std::stod(params["c_size"]);
            double rs = std::stod(params["r_size"]);
            double lc = std::stod(params["levelcap"]);
            double horizon = std::stod(params["horizon"]);
            int j = std::stoi(params["write_idx"]);
            double askStrength = 0;
            double bidStrength = 0;
            int endidx = AddedOrders.size()-1;
            int startidx = horizon_idx[horizon];
            double volume = 0;
            double numerator = 0;
            for(int i = startidx;i<endidx;i++){
                double side = AddedOrders[i].second.Side;
                double price = AddedOrders[i].second.Price;
                double price_diff = std::abs(book.Mid() - price);
                int lvl = AddedOrders[i].second.Level;
                double rsize = AddedOrders[i].second.OrigSize;
                double csize = AddedOrders[i].second.AddSize;
                if(lvl < 1 || lvl > lc) continue;

                double value = std::pow(rsize,rs) * std::pow(csize,cs) *  std::exp(-cl * price_diff);// * std::exp(-ct * (timediff - offset));
                if(side == 1 && price >= curmid){
                    askStrength += value;
                }
                if(side == -1 && price <= curmid){
                    bidStrength += value;
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

void AddSignalImba::update(Datetime d,AddOrder a){
    // if(coin == "BTC-USD" && exchange == "BNB"){
    //     std::cout << d.string() << " " << a.Price << " " << a.Side << " " << a.Level << std::endl;
    // }
    AddedOrders.push_back(std::make_pair(d,a));
}
void AddSignalImba::clear(){
    AddedOrders.clear();
}