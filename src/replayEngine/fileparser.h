#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <vector>
#include <iostream>
class FileParser{
    std::string path;
    std::string coin;
    std::string date;
    std::string exchange;
    std::string queryType;
    
    std::vector<std::vector<std::string>>data;
    int filectr;
    int rowctr;
    public:
        std::vector<std::string>orderedPaths;
        std::vector<std::string> iterate();
        bool loadCSV();
        bool isEmpty();
        std::string peakTime();
        FileParser(std::string coin, std::string exchange, std::string path, std::string queryType);
        bool loadFiles(std::string date);
    
};


#endif