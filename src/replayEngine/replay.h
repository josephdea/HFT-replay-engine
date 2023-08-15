#ifndef REPLAY_H_
#define REPLAY_H_

#include "fileparser.h"
#include <vector>
#include <cctype>
#include <string>

#include "orderbook.h"
#include "customiterator.h"
#include "bookiterator.h"
#include "marketiterator.h"
#include "tradeiterator.h"
#include "datetime.h"
#include "configparser.h"
#include "nexus.h"
#include <algorithm>
#include "ordex.h"
#include "strategy.h"
#include "taker.h"
#include "maker.h"
//class Signal;
#include "signal.h"

// template <typename Container> // we can make this generic for any container [1]
// struct container_hash {
//     std::size_t operator()(Container const& c) const {
//         return boost::hash_range(c.begin(), c.end());
//     }
// };
struct time_greater_than {
    bool operator()( std::pair<Datetime, int> const& a,std::pair<Datetime, int> const& b) const {
        Datetime x = a.first;
        Datetime y = b.first;
        return x > y;
    }
};

class Replay{
    public:
        Nexus &nexus;
        int queryRate;
        int featuresLength;
        int numSeqSignals;
        int mode;
        int predictionIdx;
        bool verbose;
        Datetime startTime;
        Datetime endTime;
        std::string path;
        std::vector<std::string>dates;
        std::string fileoutstr;
        std::string predictCoin;
        std::vector<Signal*>allSignals;
        Ordex *ordex;
        std::vector<Strategy*>allOrdexes;
        std::vector<Datetime>predictionTimes;
        std::vector<double>predictions;
        //std::unordered_map<std::vector<std::variant<std::string,double>>,int,container_hash<std::variant<std::string,double>>>signalidx;
        std::vector<std::pair<Datetime,int>> eventTimeHeap;
        typedef std::pair<Datetime,int> eventTime;
        std::priority_queue<eventTime,std::vector<eventTime>,time_greater_than>eventPQ;
        Replay();
        void SetFileOut(std::string f);
        void SetDates(std::vector<std::string> d);
        void setMarketHours(std::string start, std::string end);
        void addReturnEvent(std::string coin, std::string exchange,std::vector<int>horizons);
        void SetQueryRate(int n);
        void replayEvents();
        void SetPredictionSymbol(std::string coin);
        void loadSignals(std::string fname);
        void loadSignalIdxs();
        void setMode(std::string s);
        void loadSettings();
        void loadPredictions();
        std::vector<std::string> produceColNames();
        
};

#endif