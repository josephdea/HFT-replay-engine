#include "nexus.h"

Nexus::Nexus(std::string p):path(p){

}
void Nexus::addIterator(std::string coin, std::string exchange, std::string queryType){
    std::string id = uIdentifier(coin,exchange,queryType);
    if(allIterators.find(id) != allIterators.end()){
        return;
    }
    if(queryType == "orderbook"){
        //std::cout << id << " book placed" << std::endl;
        if(allBooks.find(id) != allBooks.end()){
            allBooks[id] = Orderbook();
        }
        //std::cout << "book" << coin << exchange << std::endl;
        allBookIterators[id] = new BookIterator(coin,exchange,path,allBooks[id]);
        allIterators[id] = allBookIterators[id];
    }
    else if(queryType == "trades"){
        //std::cout << "trades" << coin << exchange << std::endl;
        allTradeIterators[id] = new TradeIterator(coin,exchange,path);
        allIterators[id] = allTradeIterators[id];
    }
    else if(queryType == "markets"){
        //std::cout << "markets" << coin << exchange << std::endl;
        allMarketIterators[id] = new MarketIterator(coin,exchange,path);
        allIterators[id] = allMarketIterators[id];
    }
    else if(queryType == "returns"){
        //std::cout << "returns" << coin << exchange << std::endl;
        allReturnsIterators[id] = new ReturnsIterator(coin,exchange,allBooks[uIdentifier(coin,exchange,"orderbook")],std::vector<double>{5,10,15,20,25});
        allIterators[id] = allReturnsIterators[id];
        return;
        //
    }
    iterators.push_back(allIterators[id]);
    //std::cout << iterators.size() << std::endl;
}

std::string Nexus::uIdentifier(std::string coin, std::string exchange,std::string tag){
    return coin+"_"+exchange+"_"+tag;
}
// std::vector<std::string>Nexus::getColNames(){
//     std::vector<std::string>colNames;
//     colNames.push_back("time");
//     for(int i = 0;i<sign.size();i++){
//         std::vector<std::string>fpcolNames = fps[i]->getColNames();
//         colNames.insert(std::end(colNames), std::begin(fpcolNames), std::end(fpcolNames));
//     }
//     for(const auto &[key,value]:allReturnsIterators){
//         std::vector<std::string>s = allReturnsIterators[key]->getColNames();
//         colNames.insert(colNames.end(),s.begin(),s.end());

//     }

    
//     return colNames;
// }
std::string buildRowString(std::vector<double>rowData,int reqLength){
    while(rowData.size() < reqLength){
        rowData.push_back(1.01);
    }
    std::string res = "";
    for(auto it:rowData){
        res += std::to_string(it)+",";
    }
    res.pop_back();
    return res;
}
std::string buildRowString(std::vector<double>rowData){
    std::string res = ",";
    for(auto it:rowData){
        res += std::to_string(it)+",";
    }
    res.pop_back();
    return res;
}
std::string buildRowString(std::vector<std::string>rowData,int reqLength){
    while(rowData.size() < reqLength){
        rowData.push_back("");
    }
    std::string res = "";
    for(std::string &s:rowData){
        res += s+",";
    }
    res.pop_back();
    return res;
}
void Nexus::writeCSV(std::string fileOut,std::vector<std::string>colNames){
    std::cout <<"writing csv"<<std::endl;
    // std::cout << "colnames size " << colNames.size() << std::endl;
    // std::cout << features.size() << std::endl;
    std::ofstream myfile;
    myfile.open(fileOut);
    int rowLength = colNames.size();
    int featuresLength = colNames.size() - 1;
    for(const auto &[key,value]:allReturnsIterators){
        featuresLength -= allReturnsIterators[key]->getColNames().size();
    }
    myfile << buildRowString(colNames,colNames.size());
    myfile << std::endl;
    for(int i = 0;i<features.size();i++){
        myfile << timestamps[i].string();
        myfile << ",";
        myfile << buildRowString(features[i],featuresLength);
        // if(allReturnsIterators.size() != 0){
        //     myfile << ",";
        // }
        
        for(const auto &[key,value]:allReturnsIterators){
            //std::cout <<  allReturnsIterators[key]->toWrite[i].size() << " " << buildRowString(allReturnsIterators[key]->toWrite[i]).size() << " " << allReturnsIterators[key]->getColNames().size() << std::endl;
            myfile << buildRowString(allReturnsIterators[key]->toWrite[i]);
            
        }
        myfile << std::endl;
    }
}
void Nexus::writePNL(std::string fileOut,std::vector<std::string>colNames){
    std::cout <<"writing pnls"<<std::endl;
    // std::cout << "colnames size " << colNames.size() << std::endl;
    // std::cout << features.size() << std::endl;
    std::ofstream myfile;
    myfile.open(fileOut);
    myfile << "Ordex";
    for(auto it:dayStrings){
        myfile << "," + it + "_PNL";
    }
    for(auto it:dayStrings){
        myfile << "," + it + "_Volume";
    }
    for(auto it:dayStrings){
        myfile << "," + it + "_minPNL";
    }
    for(auto it:dayStrings){
        myfile << "," + it + "_zeroX";
    }
    myfile << std::endl;
    for(int i = 1;i<colNames.size();i++){
        myfile << colNames[i];
        for(int j = 0;j<EODPNL.size();j++){
            myfile << "," << EODPNL[j][i-1]; 
        }
        for(int j = 0;j<EODVolume.size();j++){
            myfile << "," << EODVolume[j][i-1]; 
        }
        for(int j = 0;j<minEODPNL.size();j++){
            myfile << "," << minEODPNL[j][i-1]; 
        }
        for(int j = 0;j<zeroCrossing.size();j++){
            myfile << "," << zeroCrossing[j][i-1]; 
        }
        myfile << std::endl;
    }
    // int rowLength = colNames.size();
    // int pnllengths = rowLength-1;
    // myfile << buildRowString(colNames,rowLength);
    // myfile << std::endl;
    // for(int i = 0;i<EODPNL.size();i++){
    //     myfile << dates[i];
    //     myfile << ",";
    //     myfile << buildRowString(EODPNL[i],pnllengths);
    //     myfile << std::endl;
    // }
}
// std::vector<CustomIterator*>iterators;
//         std::unordered_map<std::string,Orderbook>allBooks;
//         std::unordered_map<std::string,CustomIterator*>allIterators;
//         std::unordered_map<std::string,BookIterator*>allBookIterators;
//         std::unordered_map<std::string,TradeIterator*>allTradeIterators;
//         std::unordered_map<std::string,MarketIterator*>allMarketIterators;
//         std::map<std::string,ReturnsIterator*>allReturnsIterators;
// std::map<std::string, Texture*>::iterator itr = textureMap.find("some/path.png");
// if (itr != textureMap.end())
// {
//     // found it - delete it
//     delete itr->second;
//     textureMap.erase(itr);
// }

// void Nexus::clear(){
//     for(auto it:iterators){
//         delete it;
//     }
//     for(auto &[key,value]:allIterators){

//     }
//     for(auto it:allIterators){
//         delete it;
//     }
//     for(auto it:allBookIterators){
//         delete it;
//     }
//     for(auto it:allTrade)
// }