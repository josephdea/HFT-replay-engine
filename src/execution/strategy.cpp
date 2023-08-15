#include "strategy.h"

Strategy::Strategy(std::string coin, std::string exchange,std::string path, Nexus &n):ordex(Ordex(coin,exchange,path,n)){

}

std::string Strategy::getColName(){
    std::string res = strategyName + "_" + ordex.exchange + "_" + ordex.coin;
    for(auto it: paramNames){
        res += "_" + paramsList[it];
    }
    return res;
}