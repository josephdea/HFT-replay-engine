#include "reversionsignal.h"
#include "math.h"
int binary_search_lower(std::deque<std::pair<Datetime,double>>&container,Datetime d,int left, int right){
    int res = left;
    while(left <= right){
        int mid = (left+right)/2;

        if(container[mid].first > d){
            
            right = mid-1;
            
        }
        else{
            res = mid;
            left = mid + 1;
            
        }
    }
    return res;
}
int binary_search_upper1(std::deque<Trade>&container,Datetime d,int left, int right){
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

ReversionSignal::ReversionSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    std::string id = n.uIdentifier(coin,exchange,"orderbook");
    nexus.allBookIterators[id]->subscribeMidChange(boost::bind(&ReversionSignal::update,this,boost::placeholders::_1,boost::placeholders::_2));
    //id = n.uIdentifier(coin,exchange,"trades");
    //nexus.allTradeIterators[id]->subscribeTrade(boost::bind(&ReversionSignal::update,this,boost::placeholders::_1));
}
void ReversionSignal::processParams(signalconfig sc){
    this->SignalName = sc.signalname;
    this->paramNames = sc.colNamesOrdered;
    if(sc.params.find("starthorizon") == sc.params.end() || sc.params.find("endhorizon") == sc.params.end()){
        std::cout << "mis-specification of reversion signal horizons" << std::endl;
        exit(1);
    }
    if(sc.params["starthorizon"].size() != sc.params["endhorizon"].size()){
        std::cout << "mis-specification of reversion signal horizons" << std::endl;
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
void ReversionSignal::Preprocess(){//parallelize this
    int startidx = binary_search_lower(PrevMids,this->queryTime-uniqueHorizons[uniqueHorizons.size()-1],0,PrevMids.size()-1);
    for(int i = 0;i<startidx;i++){
        PrevMids.pop_front();
    }
    int left = 0;
    int right = PrevMids.size()-1;
    for(int i = uniqueHorizons.size()-1;i>=0;i--){
        double h = uniqueHorizons[i];
        left = binary_search_lower(PrevMids,this->queryTime - h,left,right);
        horizon_idx[h] = left;
    }

}
void ReversionSignal::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    //w.insert(w.end(),pllvect.begin(),pllvect.end());
    std::for_each(
        std::execution::par_unseq,
        paramsList.begin(),
        paramsList.end(),
        [&](auto &&params){
        double sh = std::stod(params["starthorizon"]);
        double eh = std::stod(params["endhorizon"]);
        int j = std::stoi(params["write_idx"]);
        int sh_idx = horizon_idx[sh];
        int eh_idx = horizon_idx[eh];
        double sh_mid = PrevMids[sh_idx].second;
        double eh_mid = PrevMids[eh_idx].second;
        double returns = (sh_mid - eh_mid) / eh_mid; 
        w[writeIdxOffset+j] = returns;
        }
    );
}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);
void ReversionSignal::update(Datetime d,double mid){
    PrevMids.push_back(std::make_pair(d,mid));
}
void ReversionSignal::update(Trade t){
    RecentTrades.push_back(t);
}
void ReversionSignal::clear(){
    RecentTrades.clear();
}