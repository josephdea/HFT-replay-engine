#include "marketiterator.h"

MarketIterator::MarketIterator(std::string coin, std::string exchange, std::string path):CustomIterator(coin,exchange,path,"orderbook"){
    this->ctr = 1;
}
void MarketIterator::iterate(Datetime d){
std::vector<std::string> s;
    if(fp.isEmpty() == false){
        s = fp.iterate();
        this->indexPrice  = std::stod(s[1]);
        this->oraclePrice = std::stod(s[2]);
        this->fundingRate = std::stod(s[3]);
        this->openInterest= std::stod(s[4]);
        
    }
}
void MarketIterator::beginDay(std::string date){
    fp.loadFiles(date);
}
void MarketIterator::endDay(std::string date){

}