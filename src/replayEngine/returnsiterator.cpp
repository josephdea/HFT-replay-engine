#include "returnsiterator.h"
#include "math.h"
ReturnsIterator::ReturnsIterator(std::string coin, std::string exchange,Orderbook &b,std::vector<double>horizons):book(b),CustomIterator(coin,exchange,"",""){
    this->horizons = horizons;
    this->ctr = 0;
}
Datetime ReturnsIterator::peakTime(){
    if(QueryHeap.size() == 0){
        std::cout <<"empty returns" << std::endl;
        //exit(1);
        return Datetime("MAX");
    }
    return std::get<0>(QueryHeap.front());
}
struct time_greater_than_tup {
    bool operator()( std::tuple<Datetime, int, double,double> const& a,std::tuple<Datetime, int, double,double> const& b) const {
        Datetime x = std::get<Datetime>(a);
        Datetime y = std::get<Datetime>(b);
        return x > y;
    }
};
std::vector<std::string> ReturnsIterator::getColNames(){
    std::vector<std::string>colNames;
    std::string baseStr = exchange+"_"+coin+"_";
    for(auto it:horizons){
        colNames.push_back(baseStr+"returns"+std::to_string(it));
        colNames.push_back(baseStr+"ssreturns"+std::to_string(it));
    }
    return colNames;
}
void ReturnsIterator::onQuery(Datetime d){
    toWrite.push_back(std::vector<double>());
    int idx = toWrite.size()-1;
    for(int h:horizons){
        Datetime futureTime(d);
        futureTime.addSeconds(h);
        //timeStruct debu = addSeconds(minTime,h);
        QueryHeap.push_back(std::make_tuple(futureTime,idx,book.BestBid(),book.BestAsk()));
        std::push_heap(QueryHeap.begin(),QueryHeap.end(),time_greater_than_tup());
    }
}
void ReturnsIterator::iterate(Datetime d){
    if(QueryHeap.size() == 0){
        std::cout << "returns iterator empty" << std::endl;
        exit(1);
    }
    int i = std::get<1>(QueryHeap.front());
    double bestBid = std::get<2>(QueryHeap.front());
    double bestAsk = std::get<3>(QueryHeap.front());
    double curbestBid = book.BestBid();
    double curbestAsk = book.BestAsk();

    double initialMid = 0.5 * (bestAsk + bestBid);
    double returns = (book.Mid() - initialMid)  / initialMid;
    double ssreturns;
    if(returns < 0){
        ssreturns = (curbestBid - bestBid) / bestBid;
    }
    else{
        ssreturns = (curbestAsk - bestAsk) / bestAsk;
    }
    toWrite[i].push_back(returns);
    toWrite[i].push_back(ssreturns);
    std::pop_heap(QueryHeap.begin(),QueryHeap.end(),time_greater_than_tup());
    QueryHeap.pop_back();
}
void ReturnsIterator::beginDay(std::string date){
    // std::cout <<"Clearing " << std::endl;
    // exit(1);
    QueryHeap.clear();
    toWrite.clear();
}
void ReturnsIterator::endDay(std::string date){
    // std::cout <<"Clearing " << std::endl;
    // exit(1);
    QueryHeap.clear();
    toWrite.clear();
}