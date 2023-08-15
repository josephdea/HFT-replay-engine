#ifndef BOOKITERATOR_H_
#define BOOKITERATOR_H_
#include "customiterator.h"
#include "orderbook.h"
#include <boost/function.hpp>
//#include "signal.h"
class BookIterator:public CustomIterator{
    public:
        Orderbook &book;
        // std::vector<void (*)(Datetime, int)>midFunctions;
        // std::vector<void (*)(Datetime, AddOrder)>addFunctions;
        // std::vector<void (*)(Datetime, CancelOrder)>cancelFunctions;
        // std::vector<void (*)(Datetime, CutinOrder)>cutFunctions;

        std::vector<boost::function<void(Datetime,double)>>midFunctions;
        std::vector<boost::function<void(Datetime,AddOrder)>>addFunctions;
        std::vector<boost::function<void(Datetime,CancelOrder)>>cancelFunctions;
        std::vector<boost::function<void(Datetime,CutinOrder)>>cutFunctions;
        BookIterator(std::string coin, std::string exchange, std::string path,Orderbook &book);
        void iterate(Datetime d);
        void beginDay(std::string date);
        void endDay(std::string date);
        // void subscribeMidChange(void (*func)(Datetime, int));
        // void subscribeAddChange(void (*func)(Datetime,AddOrder));
        // void subscribeCancelChange(void (*func)(Datetime,CancelOrder));
        // void subscribeCutinChange(void (*func)(Datetime,CutinOrder));

        void subscribeMidChange(boost::function<void(Datetime,double)>func);
        void subscribeAddChange(boost::function<void(Datetime,AddOrder)>func);
        void subscribeCancelChange(boost::function<void(Datetime,CancelOrder)>func);
        void subscribeCutinChange(boost::function<void(Datetime,CutinOrder)>func);
        void ClearFunctions();

};
#endif