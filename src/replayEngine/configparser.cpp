#include "configparser.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
namespace fs = std::filesystem;

std::vector<signalconfig> parseConfig(std::string filename){
    std::vector<signalconfig>res;
    std::vector<std::string> row;
    std::string line, word,word1;
    std::string fname = filename;
    std::fstream file(fname,std::ios::in);
    std::unordered_map<std::string,std::vector<std::string>>p;
    std::vector<std::string>colNames;
    std::string sname;
    if(file.is_open()){
         while(getline(file, line))
        {
            row.clear();
            std::stringstream str(line);
            getline(str,word,':');
            word.erase(std::remove_if(word.begin(), word.end(), ::isspace),word.end());
            if(word == "Name"){
                if(p.size() != 0){
                    res.push_back(signalconfig{.signalname=sname,.params=p,.colNamesOrdered=colNames});
                    p.clear();
                    colNames.clear();
                }
                getline(str,word,':');
                word.erase(std::remove_if(word.begin(), word.end(), ::isspace),word.end());
                sname = word;
                continue;
            }
            while(getline(str, word1, ',')){
                word1.erase(std::remove_if(word1.begin(), word1.end(), ::isspace),word1.end());
                if(word1.find("//") != std::string::npos){
                    word1 = word1.substr(0,word1.find("//"));
                    row.push_back(word1);
                    break;
                }
                row.push_back((word1));
            }
            if(word.size() != 0){
                p[word] = row;
                if(word != "paired"){
                    colNames.push_back(word);
                }
                
            }
            
        }
        if(p.size() != 0){
            res.push_back(signalconfig{.signalname=sname,.params=p,.colNamesOrdered=colNames});
        }
    }
    std::vector<signalconfig>cleanedRes;
    for(auto sc:res){
        if(sc.signalname == "GeometricSignal" || sc.signalname == "LinearModel" || sc.signalname == "Taker" || sc.signalname == "Maker"){
            cleanedRes.push_back(sc);
            continue;
        }
        if(sc.params.find("exchange") == sc.params.end() || sc.params.find("coin") == sc.params.end()){
            std::cout << "no exchange or coin parameter detected" << std::endl;
            exit(1);
        }

        for(auto e:sc.params["exchange"]){
            for(auto c:sc.params["coin"]){
                signalconfig cleanedSc = sc;
                cleanedSc.params["exchange"] = std::vector<std::string>{e};
                cleanedSc.params["coin"] = std::vector<std::string>{c};
                //std::cout << e << c << std::endl;
                cleanedRes.push_back(cleanedSc);
            }
        }

    }

    return cleanedRes;
}

std::vector<std::unordered_map<std::string,std::string>> combinations(std::unordered_map<std::string,std::vector<std::string>>&p){
    std::vector<std::vector<std::string>>stackedParams;
    std::vector<std::vector<std::string>>tmpres;
    std::vector<std::unordered_map<std::string,std::string>>res;
    std::vector<std::string>paramNames;
    for(const auto &[key,value]:p){
        paramNames.push_back(key);
        stackedParams.push_back(value);
    }
    helpercombinations(stackedParams,0,std::vector<std::string>(),tmpres);
    for(auto it:tmpres){
        std::unordered_map<std::string,std::string>param;
        for(int i = 0;i<paramNames.size();i++){
            param[paramNames[i]] = it[i];
        }
        res.push_back(param);
    }
    for(int i = 0;i<res.size();i++){
        res[i]["write_idx"] = std::to_string(i);
    }
    return res;
}
void helpercombinations(std::vector<std::vector<std::string>>array,int i, std::vector<std::string>accum,std::vector<std::vector<std::string>>&res){
    //std::cout << array.size() << " "<<i << std::endl;
    if(i == array.size()){
        res.push_back(accum);
    }
    else{
        std::vector<std::string>row = array[i];
        for(int j = 0;j<row.size();++j){
            std::vector<std::string>tmp(accum);
            tmp.push_back(row[j]);
            helpercombinations(array,i+1,tmp,res);
        }
    }
}