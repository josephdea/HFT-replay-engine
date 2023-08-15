#include "signal.h"
#include "math.h"

bool isFloat( std::string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}
Signal::Signal(Nexus &r):nexus(r){

}
 void Signal::processParams(signalconfig sc){
    this->SignalName = sc.signalname;
    this->paramNames = sc.colNamesOrdered;
    if(sc.params.find("paired") != sc.params.end()){
        sc.params.erase("paired");
        //if(std::stod(sc.params["paired"][0]) == 1){//if paired, exchange and symbol assumed to be length 1
        this->paramsList = std::vector<std::unordered_map<std::string,std::string>>();
        //loop through each parameter, add it to a vector, indexed at ctr, if ctr > len, just take last element(return error if size not 1)
        bool flag = true;
        int idx = 0;
        
        while(flag){
            
            std::unordered_map<std::string,std::string>param;
            param["write_idx"] = std::to_string(idx);
            int ctr = 0;
            for(const auto &[key,value]:sc.params){
                //error out whenever idx bigger than vector and difference bigger than 1 (but only when size isnt 1)
                if(value.size() == 1){
                    param[key] = value[0];
                }
                else{
                    if(idx < value.size()){
                        param[key] = value[idx];
                    }
                }
                if(idx >= value.size()){
                    ctr++;
                }
            }
            idx++;
            //std::cout <<"param size comparison " << param.size() << " " << sc.params.size() << std::endl;
            if(ctr == sc.params.size() || param.size() != sc.params.size()+1){
                flag = false;
                break;
            }
            else{
                paramsList.push_back(param);
            }
        }
        return;
        
        //}
    }
    this->paramsList = combinations(sc.params);

}

std::vector<std::string> Signal::getColNames(){
    std::vector<std::string>colNames;
    for(auto &it:paramsList){
        std::string s = SignalName;
        for(auto key:this->paramNames){
        //for (const auto & [ key, value ] : it) {
            if(key.compare("write_idx") != 0){
                s += "_"+it[key]; 
            }
        }
        //}
        //std::cout << s << std::endl;
        colNames.push_back(s);
    }
    return colNames;
}

std::vector<std::variant<std::string,double>>generatesignalId(std::string s){
    std::vector<std::variant<std::string,double>> id;
    std::string delimiter = "_";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        if(isFloat(token)){
            id.push_back(std::stod(token));
        }
        else{
            id.push_back(token);
        }
        s.erase(0, pos + delimiter.length());
    }
    if(isFloat(s)){
        id.push_back(std::stod(s));
    }
    else{
        id.push_back(s);
    }
    return id;
}

std::vector<std::variant<std::string,double>> Signal::signalIdentifier(int idx){
    if(idx >= paramsList.size()){
        std::cout << "error idx bigger than list of params " << std::endl;
    }
    std::vector<std::variant<std::string,double>>signalId;
    signalId.push_back(SignalName);
    for(auto it:paramNames){
        if(isFloat(paramsList[idx][it])){
            signalId.push_back(std::stod(paramsList[idx][it]));
        }
        else{
            signalId.push_back(paramsList[idx][it]);
        }
        
    }
    return signalId;
}



