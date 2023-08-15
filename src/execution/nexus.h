#ifndef NEXUS_H_
#define NEXUS_H_
#include <unordered_map>
#include <map>
#include <vector>
#include <string>

#include "bookiterator.h"
#include "tradeiterator.h"
#include "marketiterator.h"
#include "returnsiterator.h"
#include <boost/functional/hash.hpp>
//#include "ordex.h"
template <typename Container> // we can make this generic for any container [1]
struct container_hash {
    std::size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
    }
};


class Nexus{
    public:
        std::string path;
        std::vector<Datetime>timestamps;
        std::vector<std::vector<double>>features;
        std::vector<CustomIterator*>iterators;
        std::vector<std::vector<double>>EODPNL;
        std::vector<std::vector<double>>EODVolume;
        std::vector<std::vector<double>>minEODPNL;
        std::vector<std::vector<double>>zeroCrossing;
        std::vector<std::string>dayStrings;
        std::unordered_map<std::string,Orderbook>allBooks;
        std::unordered_map<std::string,CustomIterator*>allIterators;
        std::unordered_map<std::string,BookIterator*>allBookIterators;
        std::unordered_map<std::string,TradeIterator*>allTradeIterators;
        std::unordered_map<std::string,MarketIterator*>allMarketIterators;
        std::map<std::string,ReturnsIterator*>allReturnsIterators;
        std::unordered_map<std::vector<std::variant<std::string,double>>,int,container_hash<std::vector<std::variant<std::string,double>>>>signalidx;
        Nexus(std::string path);
        void addIterator(std::string coin, std::string exchange, std::string queryType);
        std::string uIdentifier(std::string coin, std::string exchange, std::string tag);
        void writeCSV(std::string fileout,std::vector<std::string>colNames);
        void writePNL(std::string fileout,std::vector<std::string>colNames);
        //void clear();
        //std::vector<std::string> getColNames();
};


#endif