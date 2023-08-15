#include "customiterator.h"


// CustomIterator::CustomIterator(std::string coin, std::string exchange){
//     this->exchange = exchange;
//     this->coin = coin;
// }
CustomIterator::CustomIterator(std::string coin, std::string exchange, std::string path, std::string queryType):fp(FileParser(coin,exchange,path,queryType)){
    this->exchange = exchange;
    this->coin = coin;
    this->ctr = 1;
}
Datetime CustomIterator::peakTime(){
    if(fp.isEmpty()){
        return Datetime("MAX");
    }else{
        return Datetime(fp.peakTime());
    }
}