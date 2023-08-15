#ifndef DTIME_H_
#define DTIME_H_

#include <string>
#include <stdlib.h>
#include<bits/stdc++.h>



class Datetime{
    
    public:
        int Year;
        int Month;
        int Day;
        int Hours;
        int Minutes;
        int Seconds;
        int Nanoseconds;
        Datetime(std::string s);
        Datetime();
        void addSeconds(double seconds);
        void validify();
        std::string string();
        bool operator <(const Datetime& y)const;
        bool operator >(const Datetime& y)const;
        bool operator <=(const Datetime& y)const;
        bool operator >=(const Datetime& y)const;
        
        bool operator ==(const Datetime& y)const;
        //Datetime operator = (const Datetime& y)const;
        Datetime operator - (const double& y)const;
        double operator - (const Datetime& y);
        double timeDiff(Datetime &y);

};
Datetime mintime(const Datetime &lhs, const Datetime &y);
bool isInfTime(Datetime d);

#endif
