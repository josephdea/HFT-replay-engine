#ifndef DDATE_H_
#define DDATE_H_

#include <string>
#include <stdlib.h>
#include<bits/stdc++.h>



class Ddate{
    
    public:
        int Year;
        int Month;
        int Day;
        Ddate(std::string s);
        std::string string();
        bool operator ==(const Ddate&y);
        bool operator !=(const Ddate&y);
        void increment();

};

std::vector<std::string>dateRange(std::string start,std::string end);

#endif
