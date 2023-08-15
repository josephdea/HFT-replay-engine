#include "ordex.h"

LimitOrder::LimitOrder(double price, double size, double side, double sizeahead){
    this->price = price;
    this->size = size;
    this->side = side;
    this->sizeahead = sizeahead;
    
}
        //BookIterator::BookIterator(std::string coin, std::string exchange, std::string path,Orderbook &book):CustomIterator(coin,exchange,path,"orderbook"),book(book){
LimitOrder::LimitOrder(){

}
Ordex::Ordex(std::string coin, std::string exchange, std::string path, Nexus &n):nexus(n), book(n.allBooks[n.uIdentifier(coin,exchange,"orderbook")]){
    this->coin = coin;
    this->exchange = exchange;
    this->oid = 0;
    this->takefee = 0.05 * 0.01;
    this->makefee = 0.02 * 0.01;
    this->initialcapital = 100000;
    this->position = 0;
    this->minpnl = 0;
    this->capital = this->initialcapital;
}
int Ordex::SendALO(double price, double size, double side){
    if(size < 0){
        std::cout << "alo with negative size issue " << std::endl;
        exit(1);
        return -1;
    }

    double sizeahead = 0;
    if(side == 1){
        int idx = book.asks.size()-2;
        while(book.asks[idx].Price < price){
            idx--;
        }
        if(book.asks[idx].Price == price){
            sizeahead = book.asks[idx].Size;
        }
        for(const auto &[key,value]:ActiveSellOrders){
            if(value.price == price){
                sizeahead += value.size;
            }
        }
        if(verbose)std::cout << "Sell Order Placed at - " << price << " size: " << size << " sizeahead: " << sizeahead << std::endl;
        ActiveSellOrders[oid] = LimitOrder(price,size,side,sizeahead);
    }
    else{
        int idx = book.bids.size()-2;
        while(book.bids[idx].Price > price){
            idx--;
        }
        if(book.bids[idx].Price == price){
            sizeahead = book.bids[idx].Size;
        }
        for(const auto &[key,value]:ActiveBuyOrders){
            if(value.price == price){
                sizeahead += value.size;
            }
        }
        ActiveBuyOrders[oid] = LimitOrder(price,size,side,sizeahead);
        if(verbose)std::cout << "Buy Order Placed at - " << price << " size: " << size << " sizeahead: " << sizeahead << std::endl;
    }
    // std::cout << "order id: " << oid << std::endl;
    int res = oid;
    oid++;
    return res;
}


void Ordex::SendIOC(double price, double size, double side){
    if(size < 0){
        std::cout << "ioc with negative size issue " << std::endl;
        return;
    }
    if(side == 1){
        if(price != book.BestAsk()){
            return;
        }
        double available_sz = book.asks[book.asks.size()-2].Size;
        double executed_sz = std::min(available_sz,size);
        position += executed_sz;
        
        capital -=  price * executed_sz;
        volume += price * executed_sz;
        if(verbose){
            std::cout << "Executed on the ask: " << executed_sz << std::endl;
            std::cout << "spent: " << price * executed_sz << std::endl;
            std::cout << "crossing fee: " << takefee * price * executed_sz << std::endl;
        }
        
        capital -= takefee * price * executed_sz;
    }
    else{
        if(price != book.BestBid()){
            return;
        }
        double available_sz = book.bids[book.bids.size()-2].Size;
        double executed_sz = std::min(available_sz,size);
        position -= executed_sz;
        
        capital +=  price * executed_sz;
        volume += price * executed_sz;
        if(verbose){
            std::cout << "Executed on the bid: " << executed_sz << std::endl;
            std::cout << "spent: " << price * executed_sz << std::endl;
            std::cout << "crossing fee: " << takefee * price * executed_sz << std::endl;
        }
        
        capital -= takefee * price * executed_sz;
    }
}
void Ordex::Cancel1Order(int oid){
    if(ActiveBuyOrders.find(oid) != ActiveBuyOrders.end()){
        ActiveBuyOrders.erase(oid);
    }
    if(ActiveSellOrders.find(oid) != ActiveSellOrders.end()){
        ActiveSellOrders.erase(oid);
    }
}

void Ordex::CancelAllOrders(){
    ActiveBuyOrders.clear();
    ActiveSellOrders.clear();
}

double Ordex::AvailableAssets(){
    return capital + book.Mid() * position;
}
double Ordex::getPNL(){
    return this->AvailableAssets() - initialcapital;
}
void Ordex::printStatus(){
    if(!verbose)return;
    std::cout << "PNL: " << std::to_string(this->AvailableAssets() - initialcapital) << " ";
    std::cout << "Position: " << std::to_string(this->position) << std::endl;
    std::cout << "Current Mid: " << book.Mid() << std::endl;
}
void Ordex::update(Trade t){
    //iterate and decrement size
    ////someone sold, buy orders disappear
    int prevsign;
    if(position == 0){
        prevsign = 0;
    }
    else if(position < 0){
        prevsign = -1;
    }
    else{
        prevsign = 1;
    }
    if(t.Side == 1){ //check for collisions along 
        std::vector<int>erasekeys;
        for(auto &[key,value]:ActiveBuyOrders){//needs to be in greatest to least, bigger buy orders will get hit first
            if(value.price >= t.Price){  //bid order below or at my level was executed, means i wud have been hit
                // if(value.price > t.Price){//someone sold at a lower price than what i listed
                //     value.sizeahead = 0;
                // }
                // std::cout << "original sizeahead: " << value.sizeahead << std::endl;
                value.sizeahead -= t.Size;
                // std::cout << "buy order ahead of me hit at " << t.Price << " for size " << t.Size << " remaining size ahead: " << value.sizeahead << std::endl;
                if(value.sizeahead < 0){
                    double executed_sz = std::min(value.size,-1 * value.sizeahead);
                    value.size += value.sizeahead;
                    position += executed_sz;
                    capital -=  value.price * executed_sz;
                    volume += value.price * executed_sz;
                    capital -= makefee * value.price * executed_sz;
                    if(verbose) std::cout << "buy limit order hit at " << value.price <<  std::endl;
                    this->printStatus();
                    
                }
                if(value.size < 0) erasekeys.push_back(key);
            }
        }
        for(auto it:erasekeys){
            ActiveBuyOrders.erase(it);
        }
    }
    else{
        std::vector<int>erasekeys;
        for(auto &[key,value]:ActiveSellOrders){ //needs to be least to biggest, lower offers get hit first
            if(value.price <= t.Price){  //bid order below or at my level was executed, means i wud have been hit
                // if(value.price < t.Price){ //someone bought at a higher price than what i offered
                //     value.sizeahead = 0;
                // }
                // std::cout << "original sizeahead: " << value.sizeahead << std::endl;
                value.sizeahead -= t.Size;
                // std::cout << "sell order ahead of me hit at " << t.Price << " for size " << t.Size << " remaining size ahead: " << value.sizeahead << std::endl;
                if(value.sizeahead < 0){
                    double executed_sz = std::min(value.size,-1 * value.sizeahead);
                    value.size += value.sizeahead;
                    position -= executed_sz;
                    capital +=  value.price * executed_sz;
                    volume += value.price * executed_sz;
                    capital -= makefee * value.price * executed_sz;
                    if(verbose)std::cout << "sell limit order hit " << value.price << std::endl;
                    this->printStatus();
                    
                }
                if(value.size < 0) erasekeys.push_back(key);
            }
        }
        for(auto it:erasekeys){
            ActiveSellOrders.erase(it);
        }
    }
    int cursign;
    if(position == 0){
        zerocrosses += abs(prevsign);
    }
    else{
        if(position < 0){
            cursign = -1;
        }
        else{
            cursign = 1;
        }
        if(cursign * prevsign < 0){
            zerocrosses += 1;
        }
    }
    this->minpnl = std::min(this->minpnl,this->getPNL());
}

void Ordex::iterate(Datetime d){

}


void Ordex::subscribeCancel(Datetime d,CancelOrder c){
    double px = c.Price;
    for(auto &[key,value]:ActiveBuyOrders){//needs to be in greatest to least, bigger buy orders will get hit first
        if(value.price == px && px != book.BestBid()){
            value.sizeahead -= c.RemoveSize;
        }
    }
    for(auto &[key,value]:ActiveSellOrders){//needs to be in greatest to least, bigger buy orders will get hit first
        if(value.price == px && px != book.BestAsk()){
            value.sizeahead -= c.RemoveSize;
        }
    }

}


void Ordex::beginDay(){
    ActiveBuyOrders.clear();
    ActiveSellOrders.clear();
    this->takefee = 0.05 * 0.01;
    this->makefee = 0.02 * 0.01;
    this->initialcapital = 100000;
    this->capital = this->initialcapital;
    this->position = 0;
    this->minpnl = 0;
    this->zerocrosses = 0;
    oid = 0;
    volume = 0;
    //positionDumper.clear();
    std::string id = nexus.uIdentifier(coin,exchange,"trades");
    std::string bookid = nexus.uIdentifier(coin,exchange,"orderbook");
    nexus.allTradeIterators[id]->subscribeTrade(boost::bind(&Ordex::update,this,boost::placeholders::_1));
    nexus.allBookIterators[bookid]->subscribeCancelChange(boost::bind(&Ordex::subscribeCancel,this,boost::placeholders::_1,boost::placeholders::_2));
}
void Ordex::endDay(){
    ActiveBuyOrders.clear();
    ActiveSellOrders.clear();
}
// std::string Ordex::getColName(){
//     std::string res = ordexname + "_" + exchange + "_" + coin;
//     for(auto it: paramNames){
//         res += "_" + paramsList[it];
//     }
//     return res;
// }

void Ordex::liquidatePosition(){
    if(position < 0){
        SendIOC(book.BestAsk(),-1*position,1);
    }
    else if(position > 0){
        SendIOC(book.BestBid(),position,-1);
    }
    this->minpnl = std::min(this->minpnl,this->getPNL());
}