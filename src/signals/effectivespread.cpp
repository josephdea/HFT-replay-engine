#include "effectivespread.h"

EffectiveSpread::EffectiveSpread(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
}
// void BooklvlSignal::Preprocess(){

// }
void EffectiveSpread::ComputeSignal(){//params,level_start,c,e,lvls,which mid
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    for(int i = 0;i<paramsList.size();i++){
        int lvl_start = std::stoi(paramsList[i]["level_start"]);
        double size = std::stod(paramsList[i]["size"]);
        double c = std::stod(paramsList[i]["c"]);
        double e = std::stod(paramsList[i]["e"]);
        int j = std::stoi(paramsList[i]["write_idx"]);
        double askNum,askDen,bidNum,bidDen;
        std::tie(askNum,askDen) = book.TraverseSidePxSzSize(lvl_start,1,size,c,e);
        std::tie(bidNum,bidDen) = book.TraverseSidePxSzSize(lvl_start,-1,size,c,e);
        double spread = (askNum/askDen) - (bidNum/bidDen);
        w[writeIdxOffset+j] = spread;
    }

}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);

void EffectiveSpread::clear(){

}