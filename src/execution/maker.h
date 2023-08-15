#include "strategy.h"
#ifndef MAKER_H_
#define MAKER_H_

class Maker:public Strategy{
    public:
        double mid_bias;
        double size;
        int spread_mode;
        double adjust_threshold;
        double spread_scaler;
        double vol_scaler;
        double pred_scaler;
        double limit_size;
        int spreadidx;
        Maker(std::string coin, std::string exchange,std::string path, Nexus &n);
        void onQuery(Datetime d, double prediction);
        void beginDay();
        void endDay();
};

#endif