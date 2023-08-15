#include "Ddate.h"

Ddate::Ddate(std::string s){
    this->Month = std::stoi(s.substr(0,2));
    this->Day = std::stoi(s.substr(3,2));
    this->Year = std::stoi(s.substr(6,4));


}
bool Ddate::operator ==(const Ddate& y){
    return std::tie(Year,Month,Day) == std::tie(y.Year,y.Month,y.Day);
}
bool Ddate::operator !=(const Ddate& y){
    return std::tie(Year,Month,Day) != std::tie(y.Year,y.Month,y.Day);
}
void Ddate::increment(){
    this->Day += 1;
    if(this->Day == 32){
        this->Month += 1;
        this->Day = 1;
    }
}
std::string Ddate::string(){
    std::string y = "" + std::to_string(Year);
    std::string M = ""+ std::to_string(Month);
    while(M.length() <2){
        M = "0" + M;
    }
    std::string d = "" + std::to_string(Day);
    while(d.length() <2){
        d = "0" + d;
    }

    std::string res = M + "-" + d + "-" + y;
    return res;
}
std::vector<std::string>dateRange(std::string start,std::string end){
    Ddate startDate = Ddate(start);
    Ddate endDate = Ddate(end);
    std::vector<std::string>res;
    while(startDate != endDate){
        res.push_back(startDate.string());
        startDate.increment();
    }
    res.push_back(endDate.string());
    return res;
}
