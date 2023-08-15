
#include "FileParser.h"
#include <filesystem>
#include <fstream>
#include <vector>
namespace fs = std::filesystem;


FileParser::FileParser(std::string c, std::string e, std::string p, std::string q):coin(c),exchange(e),path(p),queryType(q){

}

bool FileParser::loadCSV(){
    data.clear();
    rowctr = 0;
    std::vector<std::string> row;
    std::string line, word;
    if(filectr < orderedPaths.size()){
        std::string fname = orderedPaths[filectr];
        filectr++;
        std::fstream file (fname, std::ios::in);
        if(file.is_open())
        {
            while(getline(file, line))
            {
                row.clear();
    
                std::stringstream str(line);
                while(getline(str, word, ',')){
                    row.push_back(word);
                }
                    
                data.push_back(row);
            }
            file.close();
            //std::cout << filectr << " " << data.size() << std::endl;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
    if(data.size() <= 0){
        if(filectr == orderedPaths.size()){
            std::cout << orderedPaths[filectr-1];
        }
        else{
            std::cout << orderedPaths[filectr];
        }
        std::cout <<"empty file" << std::endl;
        exit(1);
    }
    if(data.size() == 1){
        loadCSV();
    }
    if(data.size() == 1 && filectr >= orderedPaths.size()){
        std::cout << "last file empty " << std::endl;
        exit(1);
    }
    return true;
}

bool FileParser::isEmpty(){
    if(filectr >= orderedPaths.size() && rowctr >= data.size()){
        return true;
    }
    return false;
}

std::string FileParser::peakTime(){

    if(rowctr == data.size()){
        // std::cout <<"current empty" << std::endl;
        // exit(1);
        bool res = loadCSV();
        if(!res){
            return "";
        }
        rowctr = 1;
    }
    std::vector<std::string>res = data[rowctr];
    return res[0];
}

std::vector<std::string> FileParser::iterate(){
    if(rowctr == data.size()){
        //std::cout <<"current empty2" << std::endl;
        bool res = loadCSV();
        if(!res){
            return std::vector<std::string>();
        }
        rowctr = 1;
    }
    if(rowctr ==0 && data.size() == 0){
        std::cout <<"incorrect data loaded " << std::endl;
    }
    //std::cout << rowctr << " " << data.size() << std::endl;
    std::vector<std::string>res = data[rowctr];
    if(res.size() == 0){
        std::cout << rowctr << std::endl;
    }
    rowctr++;
    return res;

}

bool FileParser::loadFiles(std::string date){
    orderedPaths.clear();
    data.clear();
    rowctr = 0;
    int ctr = 1;
    filectr = 0;
    std::string exchangePath = path + "/" + date + "/" + coin + "/" + exchange + "/";
    //std::cout << exchangePath +queryType + std::to_string(ctr)+".csv"<< std::endl;
    while(fs::exists(exchangePath+queryType + std::to_string(ctr)+".csv")){
        //std::cout << exchangePath +queryType + std::to_string(ctr)+".csv"<< std::endl;
        orderedPaths.push_back(exchangePath+queryType + std::to_string(ctr)+".csv");
        ctr++;
    }
    return true;
}
