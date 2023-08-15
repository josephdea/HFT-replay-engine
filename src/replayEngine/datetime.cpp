#include "datetime.h"

Datetime::Datetime(){
    
}

Datetime::Datetime(std::string s){
    if(s == "MAX"){
        Year = 4001;
        return;
    }
    try{
        Year = std::stoi(s.substr(0,4));
        Month = std::stoi(s.substr(5,2));
        Day = std::stoi(s.substr(8,2));
        Hours = std::stoi(s.substr(11,2));
        
        Minutes = std::stoi(s.substr(14,2));
        Seconds = std::stoi(s.substr(17,2));
    }
    catch (const std::invalid_argument & e) {
        std::cout << e.what() << "\n";
    }
    std::string buildStr = "";
    for(int i = 20;isdigit(s[i]);i++){
        buildStr += s[i];
    }
    while(buildStr.size() < 9){
        buildStr += "0";
    }
    Nanoseconds = std::stoi(buildStr);
}

void Datetime::validify(){

    if(Seconds >= 60){
        Minutes += Seconds/60;
        Seconds = Seconds % 60;
    }
    if(Minutes >= 60){
        Hours += Minutes/60;
        Minutes = Minutes % 60;
    }
    if(Nanoseconds < 0){
        Seconds -= 1;
        Nanoseconds += 1e9;
    }
    if(Seconds < 0){
        Minutes -= 1;
        Seconds += 60;
    }
    if(Minutes < 0){
        Minutes += 60;
        Hours -= 1;
    }
}
void Datetime::addSeconds(double seconds){
    Seconds += seconds;
    validify();
}
std::string Datetime::string(){
    std::string y = "" + std::to_string(Year);
    std::string M = ""+ std::to_string(Month);
    while(M.length() <2){
        M = "0" + M;
    }
    std::string d = "" + std::to_string(Day);
    while(d.length() <2){
        d = "0" + d;
    }
    std::string h = "" + std::to_string(Hours);
    while(h.length() <2){
        h = "0" + h;
    }
    std::string m = "" + std::to_string(Minutes);
    while(m.length() <2){
        m = "0" + m;
    }
    std::string s = "" + std::to_string(Seconds);
    while(s.length() <2){
        s = "0" + s;
    }
    std::string n = "" + std::to_string(Nanoseconds);
    while(n.length() <9){
        n = "0" + n;
    }
    std::string res = y + "-" + M + "-" + d + " " + h + ":" + m + ":" + s + "." + n;
    return res;
}

bool Datetime::operator <(const Datetime& y)const{
    if(this->Hours < y.Hours){
        return true;
    }
    else if(this->Hours > y.Hours){
        return false;
    }
    if(this->Minutes < y.Minutes){
        return true;
    }
    else if(this->Minutes > y.Minutes){
        return false;
    }
    if(this->Seconds < y.Seconds){
        return true;
    }
    else if(this->Seconds > y.Seconds){
        return false;
    }
    if(this->Nanoseconds < y.Nanoseconds){
        return true;
    }
    else if(this->Nanoseconds > y.Nanoseconds){
        return false;
    }
    return false;
}
bool Datetime::operator >(const Datetime& y)const{
    return y < *this;

    return false;
}
bool Datetime::operator <=(const Datetime& y)const{
    //std::cout << "<= operator" << std::endl;
    //std::cout << this->Hours << " " << y.Hours << std::endl;
    return (*this) < y || (*this) == y;
}
bool Datetime::operator >=(const Datetime& y)const{
    return (*this) > y || (*this) == y;
}
bool Datetime::operator ==(const Datetime& y)const{
    return this->Nanoseconds == y.Nanoseconds && this->Seconds == y.Seconds && this->Minutes == y.Minutes && this->Hours == y.Hours;
}
double Datetime::operator - (const Datetime& y){
    double myseconds =  Nanoseconds * 1e-9 + Seconds + 60 * Minutes + 3600 * Hours;
    double otherseconds =  y.Nanoseconds * 1e-9 + y.Seconds + 60 * y.Minutes + 3600 * y.Hours;
    return myseconds - otherseconds;
}
// Datetime Datetime::operator = (const Datetime& y)const{
//     Datetime res;
//     res.Month = y.Month;
//     res.Day = y.Day;
//     res.Hours = y.Hours;
//     res.Minutes = y.Minutes;
//     res.Seconds = y.Seconds;
//     res.Nanoseconds = y.Nanoseconds;
//     return res;
// }
Datetime Datetime::operator - (const double& y)const{
    Datetime res = *this;
    double whole, fractional;
    fractional = std::modf(y, &whole);
    res.Nanoseconds -= int(1e9 * fractional);
    res.Seconds -= whole;
    res.validify();
    return res;
}
Datetime mintime(const Datetime &x, const Datetime &y){
    if(x.Hours < y.Hours){
        return x;
    }
    if(x.Minutes < y.Minutes){
        return x;
    }
    if(x.Seconds < y.Seconds){
        return x;
    }
    if(x.Nanoseconds < y.Nanoseconds){
        return x;
    }
    return y;
}
bool isInfTime(Datetime d){
    return d.Year >= 3000;
}
