#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <string>
#include <vector>
#include <unordered_map>

struct signalconfig{
    std::string signalname;
    std::unordered_map<std::string,std::vector<std::string>>params;
    std::vector<std::string>colNamesOrdered;
};

std::vector<signalconfig> parseConfig(std::string filename);

void helpercombinations(std::vector<std::vector<std::string>>array,int i, std::vector<std::string>accum,std::vector<std::vector<std::string>>&res);
std::vector<std::unordered_map<std::string,std::string>> combinations(std::unordered_map<std::string,std::vector<std::string>>&p);


#endif