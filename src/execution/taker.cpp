#include "taker.h"


Taker::Taker(std::string coin, std::string exchange,std::string path, Nexus &n):Strategy(coin,exchange,path,n){

}

void Taker::onQuery(Datetime d, double prediction){
    //how to exit position? - how long to hold the position?
    // where to place my limit order (even if i have edge, i may place it so that its more likely to get hit)
    double bestoffer;
    double top_sz;
    double order_sz;
    double target_sz;
    double newmid = (1+prediction) * ordex.book.Mid();
    double size_scaler = std::stod(paramsList["size_scaler"]);
    double res_space = std::stod(paramsList["res_space"]);
    double rot = std::stoi(paramsList["rest_order_time"]);
    double adjust_diff = std::stod(paramsList["adjust_diff"]);
    int dump_mode = std::stoi(paramsList["dump_mode"]);
    int greedy_ev = std::stoi(paramsList["greedy_ev"]);
    double estimated_fees;
    double halfspread;
    double target_resting_px;
    int roid;
    if(prediction > 0){
        bestoffer = ordex.book.asks[this->ordex.book.asks.size()-2].Price;
        target_resting_px = std::ceil(newmid);
        estimated_fees = 0.05 * 0.01 * bestoffer + 0.02 * 0.01 * target_resting_px;
        top_sz = ordex.book.asks[this->ordex.book.asks.size()-2].Size;
    }
    else{
        bestoffer = ordex.book.bids[ordex.book.bids.size()-2].Price;
        target_resting_px = std::floor(newmid);
        estimated_fees = 0.05 * 0.01 * bestoffer + 0.02 * 0.01 * target_resting_px;
        top_sz = ordex.book.bids[this->ordex.book.bids.size()-2].Size;
    }
    if(prediction > 0.05 * 0.01){
        std::cout << estimated_fees << " " << abs(target_resting_px - bestoffer) << std::endl;
    }
    if(estimated_fees < abs(target_resting_px - bestoffer)){
        Datetime dumptime = d;
        if(greedy_ev) ordex.CancelAllOrders(); //want to cancel orders that 
        if(ordex.verbose)std::cout << prediction << " " << newmid << std::endl;
        if(prediction > 0){ //hit the ask, place limit order deep into ask     
            //std::cout << estimated_fees << " " << target_resting_px << " " << bestoffer << std::endl;       
            //std::cout << " top sz " << top_sz << std::endl;
            target_sz = ((abs(prediction) - 0.05 * 0.01) / 0.0001) * (1000/bestoffer) * size_scaler;
            target_sz = (1000/bestoffer);
            //target_sz = target_sz - ordex.position;
            //std::cout << "target sz " << target_sz << std::endl;
            if(greedy_ev)target_sz = std::max(0.0,target_sz - ordex.position);//if my position negative, want to buy including negative, if positive keep it and buy remaining till target
            order_sz = std::min(target_sz,top_sz);
            ordex.SendIOC(bestoffer,order_sz,1);
            if(greedy_ev){
                roid = ordex.SendALO(target_resting_px+res_space,std::abs(ordex.position),1);
            }
            else{
                roid = ordex.SendALO(target_resting_px+res_space,std::abs(order_sz),1);
            }
            dumptime.addSeconds(rot);
            positionDumper[roid] = dumptime;
            //this->SendALO(1,order_sz,std::ceil(newmid)+1);
            ordex.printStatus();
            //exit(1);
            return;
        }
        else{
            target_sz = -1 * ((abs(prediction) - 0.05 * 0.01) / 0.0001) * (1000/bestoffer) * size_scaler;//if my position positive, want to buy including positive, if negative keep it and continue selling
            if(greedy_ev) target_sz = std::min(0.0,target_sz - ordex.position);
            target_sz = std::abs(target_sz);
            target_sz = (1000/bestoffer);
            //target_sz = target_sz - ordex.position;
            order_sz = std::min(target_sz,top_sz);
            ordex.SendIOC(bestoffer,order_sz,-1);
            if(greedy_ev){
                roid = ordex.SendALO(target_resting_px-res_space,std::abs(ordex.position),-1);
            }
            else{
                roid = ordex.SendALO(target_resting_px-res_space,std::abs(order_sz),-1);
            }
            
            dumptime.addSeconds(rot);
            positionDumper[roid] = dumptime;
            //this->SendALO(1,order_sz,std::ceil(newmid)+1);
            ordex.printStatus();
            return;
        }
        // std::cout << "order id:" << roid << " placing dumptime at " << positionDumper[roid].string() << std::endl;
        
    }
    else{
        double stalePosition = 0;
        double targetmid;
        if(dump_mode == 0){
            targetmid = ordex.book.Mid();
        }
        else if(dump_mode == 1){
            targetmid = newmid;
        }
        std::vector<int>deleteKeys;
        for(const auto &[key,value]:positionDumper){
            if(positionDumper[key] < d){//dont cancel orders that are within x of the mid
                if(ordex.verbose)ordex.printStatus();
                if(ordex.ActiveBuyOrders.find(key) != ordex.ActiveBuyOrders.end()){
                    double px_diff = abs(ordex.ActiveBuyOrders[key].price - targetmid);
                    if(px_diff < adjust_diff){
                        continue;
                    }
                    stalePosition -= ordex.ActiveBuyOrders[key].size;
                    // std::cout <<"old order debug - " << positionDumper[key].string() << std::endl;
                    // std::cout << key << " " << ActiveBuyOrders[key].size << std::endl;
                    ordex.Cancel1Order(key);
                }
                else if(ordex.ActiveSellOrders.find(key) != ordex.ActiveSellOrders.end()){
                    double px_diff = abs(ordex.ActiveSellOrders[key].price - targetmid);
                    if(px_diff < adjust_diff){
                        continue;
                    }
                    stalePosition += ordex.ActiveSellOrders[key].size;
                    // std::cout <<"old order debug - " << positionDumper[key].string() << std::endl;
                    // std::cout << key << " " << ActiveSellOrders[key].size << std::endl;
                    ordex.Cancel1Order(key);
                }
                deleteKeys.push_back(key);
            }
            
        }
        for(auto it:deleteKeys){
            positionDumper.erase(it);
        }
        int roid;
        if(stalePosition > 0){
            if(dump_mode == 0){
                roid = ordex.SendALO(ordex.book.BestAsk(),stalePosition,1);
            }
            else if(dump_mode == 1){
                roid = ordex.SendALO(std::max(ordex.book.BestAsk(),std::ceil(targetmid)+res_space),stalePosition,1);
            }
            //roid = this->SendALO(book.BestAsk(),stalePosition,1);
        }
        else if(stalePosition < 0){
            if(dump_mode == 0){
                roid = ordex.SendALO(ordex.book.BestBid(),abs(stalePosition),-1);
            }
            else if(dump_mode == 1){
                roid = ordex.SendALO(std::min(ordex.book.BestBid(),std::floor(targetmid)-res_space),abs(stalePosition),-1);
            }
            // roid = this->SendALO(book.BestBid(),abs(stalePosition),-1);
        }
        if(stalePosition != 0){
            positionDumper[roid] = d;
        }
    }

    
}
void Taker::beginDay(){
    ordex.beginDay();
    ordex.verbose = std::stoi(paramsList["verbose"]);
    positionDumper.clear();
}
void Taker::endDay(){
    
}