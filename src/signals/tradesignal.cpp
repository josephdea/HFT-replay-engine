#include "tradesignal.h"

int binary_search_upper(std::deque<Trade>&container,Datetime d,int left, int right){
    int res = right+1;
    while(left <= right){
        int mid = (left+right)/2;

        if(container[mid].timeReported > d){
            res = mid;
            right = mid-1;
            
        }
        else{
            left = mid + 1;
            
        }
    }
    return res;
}

void TradeSignal::processParams(signalconfig sc){
    this->SignalName = sc.signalname;
    this->paramNames = sc.colNamesOrdered;
    
    if(sc.params.find("starthorizon") == sc.params.end() || sc.params.find("endhorizon") == sc.params.end()){
        std::cout << "mis-specification of trade signal horizons" << std::endl;
        exit(1);
    }
    if(sc.params["starthorizon"].size() != sc.params["endhorizon"].size()){
        std::cout << "mis-specification of trade signal horizons" << std::endl;
        exit(1);
    }
    if(sc.params.find("paired") != sc.params.end()){
        Signal::processParams(sc);
        for(int i = 0;i<paramsList.size();i++){
            paramsList[i]["starthorizon"] = std::to_string(std::stod(paramsList[i]["starthorizon"]));
            paramsList[i]["endhorizon"] = std::to_string(std::stod(paramsList[i]["endhorizon"]));
        }
        return;
    }
    std::vector<std::pair<double,double>>pairedHorizons;
    std::unordered_map<std::string,int>sidx_mapping;
    sc.params["pairedhorizons"] = std::vector<std::string>();
    for(int i = 0;i<sc.params["starthorizon"].size();i++){
        double sh = std::stod(sc.params["starthorizon"][i]);
        double eh = std::stod(sc.params["endhorizon"][i]);
        std::string s = sc.params["starthorizon"][i] + "_" + sc.params["endhorizon"][i];
        sc.params["pairedhorizons"].push_back(s);
        sidx_mapping[s] = pairedHorizons.size();
        pairedHorizons.push_back(std::make_pair(sh,eh));
    }
    sc.params.erase("starthorizon");
    sc.params.erase("endhorizon");
    this->paramsList = combinations(sc.params);
    for(int i = 0;i<paramsList.size();i++){
        int idx = sidx_mapping[paramsList[i]["pairedhorizons"]];
        paramsList[i]["starthorizon"] = std::to_string(pairedHorizons[idx].first);
        paramsList[i]["endhorizon"] = std::to_string(pairedHorizons[idx].second);
        paramsList[i].erase("pairedhorizons");
    }

    
}


TradeSignal::TradeSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"trades");
    nexus.allTradeIterators[id]->subscribeTrade(boost::bind(&TradeSignal::update,this,boost::placeholders::_1));
}
void TradeSignal::Preprocess(){


    int startidx = binary_search_upper(RecentTrades,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,RecentTrades.size()-1);
    for(int i = 0;i<startidx;i++){
        RecentTrades.pop_front();
    }
    int left = 0;
    int right = RecentTrades.size()-1;
    for(int i = uniqueHorizons.size()-1;i>=0;i--){
        double h = uniqueHorizons[i];
        left = binary_search_upper(RecentTrades,this->queryTime-h,left,right);
        //std::cout << "query time: " << (this->queryTime-h).string() << " " << left << std::endl;
        horizon_idx[h] = left;
    }

    // for(int i = uniqueHorizons.size()-1;i>=0;i--){
    //     double h = uniqueHorizons[i];
    //     left = binary_search_upper<AddOrder>(AddedOrders,this->queryTime - h,left,right);
    //     horizon_idx[h] = left;
    // }
}
void TradeSignal::ComputeSignal(){
    int idx = nexus.features.size()-1;
    double curmid = book.Mid();
    std::vector<double>&w = nexus.features[idx];
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
    //for(auto params:paramsList){
            double cs = std::stod(params["c_size"]);
            double ct = std::stod(params["c_prune"]);
            double cp = std::stod(params["c_px"]);
            double norm = std::stod(params["norm"]);
            double startHorizon = std::stod(params["starthorizon"]);
            double endHorizon = std::stod(params["endhorizon"]);
            double size_flag = std::stod(params["size_flag"]);
            int j = std::stoi(params["write_idx"]);
            int endidx = horizon_idx[endHorizon];
            int startidx = horizon_idx[startHorizon];
            double askStrength = 0;
            double bidStrength = 0;
            double volume = 0;
            for(int i= endidx;i<startidx;i++){
                double side = RecentTrades[i].Side;
                double size = RecentTrades[i].Size;
                if(size_flag){
                    size = pow(size,cs);
                }
                else{
                    size = size * std::exp(-size*cs);
                }
                double price = RecentTrades[i].Price;
                double pricediff = std::abs(price - curmid);
                double value = size *  std::exp(-cp * pricediff); //test different weightings
                //std::cout << value << " " << size << " " << pricediff << " " << price << " " << curmid << " " << RecentTrades.size() << std::endl;
                volume += value;
                if(ct == 1){
                    if(side == 1 && price >= curmid){
                        askStrength += value;
                }
                    if(side == -1 && price <= curmid){
                        bidStrength += value;
                    }
                }
                else{
                    if(side == 1.0){
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
                res = (askStrength - bidStrength) / (volume+ 1e-8);
            }
            else if(norm == 3){
                res = askStrength - bidStrength;
            }
            if(isnan(res) || isinf(res)){
                res = 0;
            }
            w[writeIdxOffset+j] = res;
            // std::cout << res << std::endl;
            // exit(1);
            }   
    );
}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
void TradeSignal::update(Trade t){
    RecentTrades.push_back(t);
}

void TradeSignal::clear(){
    RecentTrades.clear();
}