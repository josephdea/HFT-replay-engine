#include "bookprintsignal.h"

BookPrintSignal::BookPrintSignal(std::string coin, std::string exchange, Nexus &n):Signal(n),book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    //book = &n.allBooks[n.uIdentifier(coin,exchange,"orderbook")];
    this->coin = coin;
    this->exchange = exchange;
    
}
void BookPrintSignal::ComputeSignal(){
    int idx = nexus.features.size()-1;
    std::vector<double>&res = nexus.features[idx];
    int lvls = std::stoi(paramsList[0]["levels"]);
    int i= 0;
    int bidLimit = std::min(lvls,(int)book.bids.size()-2);
    for(i = 0;i<bidLimit;i++){
        double px = book.bids[book.bids.size()-2-i].Price;
        double sz = book.bids[book.bids.size()-2-i].Size;
        double numOrders = book.bidNumbers[px];
        res[writeIdxOffset + 3*i] = px;
        res[writeIdxOffset + 3*i + 1] = sz;
        res[writeIdxOffset + 3*i + 2] = numOrders;
        // res.push_back((px));
        // res.push_back((sz));
        // res.push_back((numOrders));
    }

    for(;i<lvls;i++){
        res[writeIdxOffset + 3*i] = 0;
        res[writeIdxOffset + 3*i + 1] = 0;
        res[writeIdxOffset + 3*i + 2] = 0;
        // res.push_back(0);
        // res.push_back(0);
        // res.push_back(0);
    }
    int askLimit = std::min(lvls,(int)book.asks.size()-2);
    for(i = 0;i<askLimit;i++){
        double px = book.asks[book.asks.size()-2-i].Price;
        //std::cout << "ask " << px << std::endl;
        double sz = book.asks[book.asks.size()-2-i].Size;
        double numOrders = book.askNumbers[px];
        res[writeIdxOffset + 3*(i+lvls)] = px;
        res[writeIdxOffset + 3*(i+lvls) + 1] = sz;
        res[writeIdxOffset + 3*(i+lvls) + 2] = numOrders;
        // res.push_back(px);
        // res.push_back(sz);
        // res.push_back(numOrders);
    }
    for(;i<lvls;i++){
        res[writeIdxOffset + 3*(i+lvls)] = 0;
        res[writeIdxOffset + 3*(i+lvls) + 1] = 0;
        res[writeIdxOffset + 3*(i+lvls) + 2] = 0;
        // res.push_back(0);
        // res.push_back(0);
        // res.push_back(0);
    }
}
// std::vector<double>BookPrintSignal::Compute(std::unordered_map<std::string,std::string>&params){
    
// }
std::vector<std::string> BookPrintSignal::getColNames(){
    std::vector<std::string>columns;
    //std::cout << "params list size " << paramsList.size() << std::endl;
    int lvls = std::stoi(paramsList[0]["levels"]);
    for(int i = 0;i<lvls;i++){
        columns.push_back(coin+"_"+exchange+"_bid_px_" + std::to_string(i));
        columns.push_back(coin+"_"+exchange+"_bid_sz_" + std::to_string(i));
        columns.push_back(coin+"_"+exchange+"_bid_orders_" + std::to_string(i));
    }
    for(int i = 0;i<lvls;i++){
        columns.push_back(coin+"_"+exchange+"_ask_px_" + std::to_string(i));
        columns.push_back(coin+"_"+exchange+"_ask_sz_" + std::to_string(i));
        columns.push_back(coin+"_"+exchange+"_ask_orders_" + std::to_string(i));
    }
    
    return columns;
}
void BookPrintSignal::clear(){
}