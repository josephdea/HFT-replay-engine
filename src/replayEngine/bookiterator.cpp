#include "bookiterator.h"
#include "math.h"
BookIterator::BookIterator(std::string coin, std::string exchange, std::string path,Orderbook &book):CustomIterator(coin,exchange,path,"orderbook"),book(book){
    book.midChangeFlag = false;
    book.addFlag = false;
    book.cancelFlag = false;
    book.cutinFlag = false;

    this->ctr = 1;
}
void BookIterator::iterate(Datetime d){
    //std::cout <<"orderbook iterate" << std::endl;
    //fp.iterate();
    // book.Update(fp.iterate());
    // return;
    if(fp.isEmpty() == false){
        //std::cout <<"book update " << d.string() << std::endl;
        book.Update(fp.iterate());
        if(book.initFlag == true){
            std::cout << "initializing book " << std::endl;
            while(fp.isEmpty() == false && book.initFlag == true){
                book.Update(fp.iterate());
            }
            book.prevMid = book.Mid();
            book.midChangeFlag = true;
            
            // std::cout << "finished initialization book" << std::endl;
            // std::cout << book.prevMid << std::endl;
            // std::cout << book.BestAsk() << " " << book.BestBid() << std::endl;
            // std::cout << coin << " " << exchange << std::endl;
            // if(coin == "BTC-USD" && exchange == "BNB"){
            //     exit(1);
            // }
            
        }
    }
    
    if(book.midChangeFlag){
        book.midChangeFlag = false;
        for(int i = 0;i<midFunctions.size();i++){
            midFunctions[i](d,book.Mid());
        }
    }
    if(book.addFlag){
        book.addFlag = false;
        for(int i = 0;i<addFunctions.size();i++){
            addFunctions[i](d,book.recentAdd);
        }
    }
    if(book.cancelFlag){
        book.cancelFlag = false;
        for(int i = 0;i<cancelFunctions.size();i++){
            cancelFunctions[i](d,book.recentCancel);
        }
    }
    if(book.cutinFlag){
        book.cutinFlag = false;
        for(int i = 0;i<cutFunctions.size();i++){
            cutFunctions[i](d,book.recentCutIn);
        }
    }
}
void BookIterator::beginDay(std::string date){
    std::cout << "book begin day " << std::endl;
    book.Clear();
    fp.loadFiles(date);
}
void BookIterator::endDay(std::string date){
    book.Clear();
}
//void (Signal::*func)(Datetime, int));
//void SomeMethod(Base& object, void (Base::*ptr)())

void BookIterator::subscribeMidChange(boost::function<void(Datetime,double)>func){
    midFunctions.push_back(func);
}
void BookIterator::subscribeAddChange(boost::function<void(Datetime,AddOrder)>func){
    addFunctions.push_back(func);
}
void BookIterator::subscribeCancelChange(boost::function<void(Datetime,CancelOrder)>func){
    cancelFunctions.push_back(func);
}
void BookIterator::subscribeCutinChange(boost::function<void(Datetime,CutinOrder)>func){
    cutFunctions.push_back(func);
}
void BookIterator::ClearFunctions(){
    midFunctions.clear();
    addFunctions.clear();
    cancelFunctions.clear();
    cutFunctions.clear();
}