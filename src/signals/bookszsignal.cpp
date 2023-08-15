#include "bookszsignal.h"

BookszSignal::BookszSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
}
// void BooklvlSignal::Preprocess(){

// }
void BookszSignal::ComputeSignal(){//params,level_start,c,e,lvls,which mid
    int idx = nexus.features.size()-1;
    std::vector<double>&w = nexus.features[idx];
    for(int i = 0;i<paramsList.size();i++){
        int lvl_start = std::stoi(paramsList[i]["level_start"]);
        double size = std::stod(paramsList[i]["size"]);
        double c = std::stod(paramsList[i]["c"]);
        double e = std::stod(paramsList[i]["e"]);
        std::string midExchange = paramsList[i]["mid_exchange"];
        int j = std::stoi(paramsList[i]["write_idx"]);
        double askNum,askDen,bidNum,bidDen;
        //std::cout << lvl_start << " " << size << " " << c << " " << e << midExchange << std::endl;
        std::tie(askNum,askDen) = book.TraverseSidePxSzSize(lvl_start,1,size,c,e);
        std::tie(bidNum,bidDen) = book.TraverseSidePxSzSize(lvl_start,-1,size,c,e);
        //std::cout << askNum << " " << askDen << " " << bidNum << " " << bidDen << std::endl;
        // if(nexus.allBooks.find(nexus.uIdentifier(coin,midExchange,"orderbook")) == nexus.allBooks.end()){
        //     std::cout << "mid book not found " << std::endl;
        //     std::cout << nexus.uIdentifier(coin,midExchange,"orderbook") << std::endl;
        // }
        //std::cout << nexus.allBooks[nexus.uIdentifier(coin,midExchange,"orderbook")].asks.size() << std::endl;
        double curmid = nexus.allBooks[nexus.uIdentifier(coin,midExchange,"orderbook")].Mid();
        //std::cout << curmid << std::endl;
        double newmid = 0.5 * (askNum/askDen) + 0.5 * (bidNum/bidDen);
        w[writeIdxOffset+j] = (newmid-curmid)/curmid;
    }

}
//std::vector<double> Compute(std::unordered_map<std::string,std::string>&params);

void BookszSignal::clear(){

}