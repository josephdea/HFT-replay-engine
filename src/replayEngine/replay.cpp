#include "replay.h"
#include "cancelsignalimba.h"
#include "addsignalmid.h"
#include "addsignalimba.h"
#include "tradesignal.h"
#include "cutinsignal.h"
#include "bookprintsignal.h"
#include "reversionsignal.h"
#include "effectivespread.h"
#include "geometricsignal.h"
#include "booklvlsignal.h"
#include "bookszsignal.h"
#include "linearsignal.h"
#include "cancelsignalmid.h"
#include "bookordersignal.h"
#include <boost/algorithm/string.hpp>
#include "execution"
#include <algorithm>
#include "Ddate.h"
namespace fs = std::filesystem;
// Signal::Signal(Replay &r):nexus(r){

// }
// void Signal::ComputeSignal(){
//     for(auto &it:paramsList){
//         Compute(it);
//     }
// }

// std::vector<std::string> Signal::getColNames(){
//     std::vector<std::string>colNames;
//     for(auto &it:paramsList){
//         std::string s = SignalName;
//         for (const auto & [ key, value ] : it) {
//             s += "_"+std::to_string(value); 
//         }
//         colNames.push_back(s);
//     }
//     return colNames;
// }
Replay::Replay():eventPQ(std::priority_queue<eventTime,std::vector<eventTime>,time_greater_than>()),nexus(*(new Nexus(""))){
    this->verbose = true;
}
void Replay::SetFileOut(std::string f){
    this->fileoutstr = f;
}
void Replay::SetDates(std::vector<std::string> d){
    dates = d;
}
void Replay::setMarketHours(std::string start, std::string end){
    startTime = Datetime(start);
    endTime = Datetime(end);
}
void Replay::addReturnEvent(std::string coin, std::string exchange,std::vector<int>horizons){

}
void Replay::SetQueryRate(int n){
    this->queryRate = n;
}

void Replay::replayEvents(){
    this->loadSettings();
    for(std::string day:dates){
        if(mode == 0){
            SetFileOut("./gridsearch/"+day+".csv");
        }
        else if(mode == 1 || mode == 3){
            SetFileOut("./gridsearch/debugc"+day+".csv");
        }
        else if(mode == 2){//order execution simulation
            auto horizon = "15";
            if(fs::exists("./signals/" + day+"signals"+horizon+".config") == false){
                std::cout <<"ERROR: signals config for ordex simulation doesn't exist : " + day << std::endl;
                std::cout <<"skipping this day " << std::endl;
                continue;
            }
            for(auto it:allSignals){
                delete it;
            }
            allSignals.clear();
            SetFileOut("./gridsearch/ordex"+day+".csv");
            nexus.signalidx.clear();
            this->loadSignals("./signals/" + day+"signals"+horizon+".config");
            nexus.features.clear();
            nexus.timestamps.clear();
            if(allOrdexes.size() == 0){
                std::vector<signalconfig> signals = parseConfig("./ordex/ordex.config");
                for(auto it:signals){
                    auto ordexparamslist = combinations(it.params);
                    for(auto param:ordexparamslist){
                        if(it.signalname == "Taker"){
                            Taker *t = new Taker("BTC-USD","DYDX",path,nexus);
                            allOrdexes.push_back(t);
                        }
                        else if(it.signalname == "Maker"){
                            Maker *t = new Maker("BTC-USD","DYDX",path,nexus);
                            allOrdexes.push_back(t);
                        }
                        allOrdexes[allOrdexes.size()-1]->paramsList = param;
                        allOrdexes[allOrdexes.size()-1]->strategyName = it.signalname;
                        allOrdexes[allOrdexes.size()-1]->paramNames = it.colNamesOrdered;
                    }
                }
            }
            nexus.EODPNL.push_back(std::vector<double>(allOrdexes.size(),0));
            nexus.minEODPNL.push_back(std::vector<double>(allOrdexes.size(),0));
            nexus.EODVolume.push_back(std::vector<double>(allOrdexes.size(),0));
            nexus.zeroCrossing.push_back(std::vector<double>(allOrdexes.size(),0));
        }
        std::cout << "day " << day << std::endl;
        while(!eventPQ.empty()){
            eventPQ.pop();
        }
        for(int i = 0;i<nexus.iterators.size();i++){
            nexus.iterators[i]->beginDay(day);
            if(!isInfTime(nexus.iterators[i]->peakTime())){
                eventPQ.push(std::make_pair(nexus.iterators[i]->peakTime(),i));
            } 
        }
        for(int i = 0;i<allOrdexes.size();i++){
            allOrdexes[i]->beginDay();
        }
        int ctr = 0;
        Datetime LaxEndTime = endTime; //allow proper flushing of events
        Datetime prevTime;
        bool beginQuerying;
        LaxEndTime.addSeconds(30);
        if(eventPQ.size() == 0){
            goto clearday;
        }
        prevTime = eventPQ.top().first;
        beginQuerying = false;
        
        while(eventPQ.size() !=0 && eventPQ.top().first <= LaxEndTime && (mode != 3 || predictionIdx < 5000)){

            //std::cout << eventPQ.top().second << std::endl;
            Datetime mintime = eventPQ.top().first;
            if(mintime.Year > 2500){
                break;
            }
            //std::cout << mintime.string() << std::endl;
            int iterIdx = eventPQ.top().second;
            prevTime = mintime;
            if(mode != 3) ctr += nexus.iterators[iterIdx]->ctr;
            //ctr += 1;
            nexus.iterators[iterIdx]->iterate(mintime);
            eventPQ.pop();
            if(!isInfTime(nexus.iterators[iterIdx]->peakTime())){
                eventPQ.push(std::make_pair(nexus.iterators[iterIdx]->peakTime(),iterIdx));
            }
            //if(ctr >= queryRate){
            //std::cout << mintime.string() << " " << predictionTimes[predictionIdx].string() << std::endl;
            // if(mintime > predictionTimes[61]) exit(1);
            // if(mintime >= predictionTimes[56] && mintime <= predictionTimes[61]){
            if(ctr >= queryRate && (beginQuerying || mintime >= startTime) && mintime <= endTime || (mode == 3 && mintime >= predictionTimes[predictionIdx])){

                if(verbose) std::cout << "query time " <<  mintime.string()  << std::endl;
                
                if(beginQuerying == false){
                    // std::cout << nexus.allReturnsIterators.size() << std::endl;
                    // exit(1);
                    for(const auto &[key,_]:nexus.allReturnsIterators){
                        nexus.iterators.push_back(nexus.allReturnsIterators[key]);
                        nexus.iterators[nexus.iterators.size()-1]->onQuery(mintime);
                        eventPQ.push(std::make_pair(nexus.iterators[nexus.iterators.size()-1]->peakTime(),nexus.iterators.size()-1));
                    }
                    beginQuerying = true;
                }
                else{
                    for(const auto &[key,_]:nexus.allReturnsIterators){
                        nexus.allReturnsIterators[key]->onQuery(mintime);
                    }
                }
                ctr = 0;
                if(mode != 2 || nexus.features.size() == 0){ //need to remove this if debugging ordex
                    nexus.timestamps.push_back(mintime);
                    nexus.features.push_back(std::vector<double>(featuresLength,0));
                }
                std::for_each(
                    std::execution::par_unseq,
                    allSignals.begin(),
                    allSignals.end()-numSeqSignals,
                    [&](auto &&signalIt){
                        signalIt->queryTime = mintime;
                        signalIt->Preprocess();
                        signalIt->ComputeSignal();
                    }
                );
                std::for_each(
                    std::execution::seq,
                    allSignals.end()-numSeqSignals,
                    allSignals.end(),
                    [&](auto &&signalIt){
                        signalIt->queryTime = mintime;
                        signalIt->Preprocess();
                        signalIt->ComputeSignal();
                    }
                );
                if(mode == 3){
                    // auto xc = produceColNames();
                    auto t = produceColNames();
                    // for(int x = 0;x<nexus.features[nexus.features.size()-1].size();x++){
                    //     std::cout << t[x+1] << " " << nexus.features[nexus.features.size()-1][x] << std::endl;
                    // }
                    // exit(1);
                    //std::cout << nexus.features[nexus.features.size()-1][nexus.features[nexus.features.size()-1].size()-1] << " " << predictions[predictionIdx] << std::endl;
                    //double diff = nexus.features[nexus.features.size()-1][nexus.features[nexus.features.size()-1].size()-1] - predictions[predictionIdx];
                    //std::cout << mintime.string() << " " << predictionTimes[predictionIdx].string() << std::endl;
                    //std::cout <<"debugging " << std::endl;
                    //std::cout << mintime.string() << " " << nexus.features[nexus.features.size()-1][0] << std::endl;
                    
                    if(verbose){
                    
                        // std::cout << predictionIdx << std::endl;
                        // for(int x = 0;x<nexus.features[nexus.features.size()-1].size();x++){
                        //     std::cout << t[x+1] << " " << nexus.features[nexus.features.size()-1][x] << std::endl;
                        // }
                    }
                    //std::cout << "diff: " << diff << std::endl;
                    predictionIdx++;
                }
                
                if(mode == 2){
                    std::for_each(
                        std::execution::par_unseq,
                        allOrdexes.begin(),
                        allOrdexes.end(),
                        [&](auto &&ordexit){
                            ordexit->onQuery(mintime,nexus.features[nexus.features.size()-1][featuresLength-1]);
                        }

                    );
                }

            }
        }
        nexus.dayStrings.push_back(day);
        for(int i = 0;i<allOrdexes.size();i++){
            int idx = nexus.EODPNL.size()-1;
            //this->AvailableAssets() - initialcapital
            allOrdexes[i]->ordex.liquidatePosition();
            nexus.EODPNL[idx][i] = allOrdexes[i]->ordex.getPNL();
            nexus.EODVolume[idx][i] = allOrdexes[i]->ordex.volume;
            nexus.minEODPNL[idx][i] = allOrdexes[i]->ordex.minpnl;
            nexus.zeroCrossing[idx][i] = allOrdexes[i]->ordex.zerocrosses;
        }

        if(mode != 2)nexus.writeCSV(fileoutstr,produceColNames());
        for(int i = 0;i<nexus.allReturnsIterators.size();i++){
            nexus.iterators[nexus.iterators.size()-1]->endDay(day);
            nexus.iterators.pop_back();
        }
    clearday:
        nexus.features.clear();
        nexus.timestamps.clear();
        for(int i = 0;i<nexus.iterators.size();i++){
            nexus.iterators[i]->endDay(day);
            if(mode == 2) nexus.iterators[i]->ClearFunctions();
        }
        
        for(int i = 0;i<allSignals.size();i++){
            allSignals[i]->clear();
        }
    }
    if(mode == 2){
        std::vector<std::string>colnames{"date"};
        for(int i = 0;i<allOrdexes.size();i++){
            colnames.push_back(allOrdexes[i]->getColName());
        }
        nexus.writePNL("./ordex/results15.csv",colnames);
    }
}

std::vector<double> uniqueHorizons(std::unordered_map<std::string,std::vector<std::string>>&p){
    //s1.find(s2) != std::string::npos
    std::set<double>set_h;
    for(const auto &[key,value]:p){
        if(boost::algorithm::to_lower_copy(key).find("horizon") != std::string::npos){
            for(auto it:value){
                set_h.insert(std::stod(it));
            }
        }
    }
    std::vector<double>res(set_h.begin(),set_h.end());
    sort(res.begin(),res.end());
    // for(int i= 0;i<res.size();i++){
    //     std::cout <<"horizon = " << res[i] << std::endl;
    // }
    return res;
}
void Replay::SetPredictionSymbol(std::string coin){
    this->predictCoin = coin;
}
void Replay::loadSettings(){
//     Name: Replay
// exchange: DYDX
// coin: BTC-USD
// Data_Path: ../../live-trading/data
// Start_MarketTime: 12412 //12312,124124
// End_MarketTime: 12412
// Start_Date:12412
// End_Date:12412
// Mode:12412
// QueryRate: 12412
    signalconfig signals = parseConfig("./replay.config")[0];
    std::string predictionCoin = signals.params["coin"][0];
    std::string dataPath = signals.params["Data_Path"][0];
    std::string startTime = "2023-03-09 " + signals.params["Start_MarketTime"][0];
    std::string endTime = "2023-03-09 " + signals.params["End_MarketTime"][0];
    std::string startDate = signals.params["Start_Date"][0];
    std::string endDate = signals.params["End_Date"][0];
    std::string mode = signals.params["Mode"][0]; //gridsearch,debug,ordex
    std::string queryrate = signals.params["QueryRate"][0];
    std::string signalConfigPath = signals.params["SignalConfigPath"][0];
    std::string verbose = signals.params["Verbose"][0];
    std::cout << predictionCoin << std::endl;
    std::cout << dataPath << std::endl;
    std::cout << startTime << std::endl;
    std::cout << endTime << std::endl;
    std::cout << startDate << std::endl;
    std::cout << endDate << std::endl;
    std::cout << mode << std::endl;
    std::cout << queryrate << std::endl;
    std::cout << signalConfigPath << std::endl;

    this->SetPredictionSymbol(predictionCoin);
    this->path = dataPath;
    this->nexus.path = dataPath;
    this->setMarketHours(startTime,endTime);
    this->SetDates(dateRange(startDate,endDate));
    this->setMode(mode);
    this->SetQueryRate(std::stoi(queryrate));
    this->verbose = bool(std::stoi(verbose));
    if(mode != "ordex"){
        this->loadSignals(signalConfigPath);
    }
    if(mode == "golangcheck"){
        this->loadPredictions();
    }
    // exit(1);

}
void Replay::loadSignals(std::string fname){
    std::cout << "loading signals" << std::endl;

    std::vector<signalconfig> signals = parseConfig(fname);
    int writeIdxOffset = 0;
    //std::unordered_map<std::vector<int>,int,container_hash<int>>signalidx;
    numSeqSignals = 0;
    for(auto it:signals){
        std::string exchange,coin;
        if(it.params.find("exchange") != it.params.end() && it.params.find("coin") != it.params.end()){
            exchange = it.params["exchange"][0];
            coin = it.params["coin"][0];
            //std::cout << exchange << coin << std::endl;
            nexus.addIterator(coin,exchange,"orderbook");
            nexus.addIterator(coin,exchange,"trades");
            //std::cout << coin << "," <<predictCoin << std::endl;
            //std::cout << "debugging " << coin << " " << predictCoin << " "<< mode << std::endl;
            if(coin == predictCoin && mode != 2){
                nexus.addIterator(coin,exchange,"returns");
                //std::cout << "adding returns " << coin << " " << exchange << std::endl;
            }
        }
        //std::cout << "signal: " << it.signalname << std::endl;
        //combinations(it.params);
        if(it.signalname == "BookPrint"){
            BookPrintSignal *s = new BookPrintSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "CancelSignalImba"){
            CancelSignalImba *s = new CancelSignalImba(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "CancelSignalMid"){
            CancelSignalMid *s = new CancelSignalMid(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "AddSignalImba"){
            AddSignalImba *s = new AddSignalImba(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "AddSignalMid"){
            AddSignalMid *s = new AddSignalMid(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "TradeSignal"){
            TradeSignal *s = new TradeSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "CutinSignal"){
            CutinSignal *s = new CutinSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "ReversionSignal"){
            ReversionSignal *s = new ReversionSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "EffectiveSpread"){
            EffectiveSpread *s = new EffectiveSpread(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "BookSignalSizeMid"){
            BookszSignal *s = new BookszSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "BookSignalLevelsMid"){
            BooklvlSignal *s = new BooklvlSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "BookSignalNumOrders"){
            BookordersSignal *s = new BookordersSignal(coin,exchange,nexus);
            allSignals.push_back(s);
        }
        else if(it.signalname == "GeometricSignal"){ //there should only be one geometric signal
            numSeqSignals++;
            loadSignalIdxs();
            GeometricSignal *s = new GeometricSignal(nexus);
            allSignals.push_back(s);
            allSignals[allSignals.size()-1]->processParams(it);
            allSignals[allSignals.size()-1]->writeIdxOffset = writeIdxOffset;
            writeIdxOffset += allSignals[allSignals.size()-1]->getColNames().size();
            continue;
        }//TODO:LINEAR REGRESSION SIGNAL
        else if(it.signalname == "LinearModel"){
            numSeqSignals++;
            loadSignalIdxs();
            LinearSignal *s = new LinearSignal(nexus);
            allSignals.push_back(s);
            allSignals[allSignals.size()-1]->processParams(it);
            allSignals[allSignals.size()-1]->writeIdxOffset = writeIdxOffset;
            writeIdxOffset += allSignals[allSignals.size()-1]->getColNames().size();
            continue;

        }
        auto uh = uniqueHorizons(it.params);
        allSignals[allSignals.size()-1]->uniqueHorizons = uh;
        allSignals[allSignals.size()-1]->processParams(it);
        allSignals[allSignals.size()-1]->writeIdxOffset = writeIdxOffset;
        //std::cout << allSignals[allSignals.size()-1]->getColNames().size() << " Signals Loaded" << std::endl;
        writeIdxOffset += allSignals[allSignals.size()-1]->getColNames().size();
        //std::cout << writeIdxOffset << std::endl;
        
    }

    featuresLength = writeIdxOffset;
}
void debug(std::vector<std::variant<std::string,double>> it){
    for(auto it1:it){
        std::visit([](auto&& arg){ std::cout << arg << " "; }, it1);
        //std::cout << it1 << std::endl;
    }
    std::cout << std::endl;
}
void Replay::loadSignalIdxs(){
    for(int i = 0;i<allSignals.size();i++){
        if(allSignals[i]->SignalName == "GeometricSignal"){
            for(int j = 0;j<allSignals[i]->paramsList.size();j++){
                std::vector<std::variant<std::string,double>>signalId;
                signalId.push_back(allSignals[i]->SignalName);
                auto x = generatesignalId(allSignals[i]->paramsList[j]["inc_signal"]);
                signalId.insert(signalId.end(),x.begin(),x.end());
                this->nexus.signalidx[signalId] = allSignals[i]->writeIdxOffset+j;
            }
            continue;
        }
        for(int j = 0;j<allSignals[i]->paramsList.size();j++){
            std::vector<std::variant<std::string,double>>signalId = allSignals[i]->signalIdentifier(j);
            //std::cout <<allSignals[i]->writeIdxOffset + j<<" ";
            //debug(signalId);
            if(this->nexus.signalidx.find(signalId) != this->nexus.signalidx.end() && this->nexus.signalidx[signalId] != (allSignals[i]->writeIdxOffset+j)){
                std::cout << "ERROR signal id collision" << std::endl;
                std::cout << allSignals[i]->SignalName << std::endl;
            }
            this->nexus.signalidx[signalId] = allSignals[i]->writeIdxOffset + j;
        }
    }
    //this->nexus.signalidx = this->signalidx;
}


void Replay::setMode(std::string s){
    if(s == "gridsearch"){
        mode = 0;
    }
    else if(s == "debug"){
        mode = 1;
    }
    else if(s == "ordex"){
        mode = 2;
    }
    else if(s == "golangcheck"){
        mode = 3;
    }
}

std::vector<std::string> Replay::produceColNames(){
    std::vector<std::string>colNames;
    colNames.push_back("time");
    for(int i = 0;i<allSignals.size();i++){
        std::vector<std::string>s = allSignals[i]->getColNames();
        colNames.insert(colNames.end(),s.begin(),s.end());
    }
    for(const auto &[key,value]:nexus.allReturnsIterators){
        std::vector<std::string>s = nexus.allReturnsIterators[key]->getColNames();
        colNames.insert(colNames.end(),s.begin(),s.end());
    }
    return colNames;

}

void Replay::loadPredictions(){
    std::vector<std::vector<std::string>>data;
    data.clear();
    std::vector<std::string> row;
    std::string line, word;
    std::string fname = "../../live-trading/predictions.csv";
    std::fstream file (fname, std::ios::in);
    getline(file,line);
    while(getline(file,line)){
        row.clear();
        std::stringstream str(line);
        while(getline(str,word,',')){
            row.push_back(word);
        }
        auto dt = Datetime(row[0]);
        // std::cout << row[1] << std::endl;
        // std::cout << row[row.size()-1] << std::endl;
        double p = std::stod(row[row.size()-1]);
        predictionTimes.push_back(dt);
        predictions.push_back(p);
        //std::cout << dt.string() << " " << p << std::endl;
        //data.push_back(row);
    }
    this->predictionIdx = 0;
    file.close();

}