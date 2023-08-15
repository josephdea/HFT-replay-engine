#include "bookordersignal.h"

BookordersSignal::BookordersSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
}
// void BooklvlSignal::Preprocess(){

// }
void BookordersSignal::ComputeSignal(){//params,level_start,c,e,lvls,which mid
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    for(int i = 0;i<paramsList.size();i++){
        int lvl_start = std::stoi(paramsList[i]["level_start"]);
        int lvl_cap = std::stoi(paramsList[i]["levels"]);
        double size = std::stod(paramsList[i]["size"]);
        double c = std::stod(paramsList[i]["c"]);
        double e = std::stod(paramsList[i]["e"]);
        std::string midExchange = paramsList[i]["mid_exchange"];
        int j = std::stoi(paramsList[i]["write_idx"]);
        double askStrength,bidStrength;
        askStrength = book.TraverseSideNumOrdersLevels(lvl_start,1,lvl_cap,c,e);
        bidStrength = book.TraverseSideNumOrdersLevels(lvl_start,-1,lvl_cap,c,e);
        // double askNum,askDen,bidNum,bidDen;
        // std::tie(askNum,askDen) = book.TraverseSidePxSzSize(lvl_start,1,size,c,e);
        // std::tie(bidNum,bidDen) = book.TraverseSidePxSzLevels(lvl_start,-1,size,c,e);
        // double curmid = nexus.allBooks[nexus.uIdentifier(coin,midExchange,"orderbook")].Mid();
        // double newmid = (askNum/askDen) - (bidNum/bidDen);
        w[writeIdxOffset+j] = askStrength - bidStrength;
    }

}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);

void BookordersSignal::clear(){

}