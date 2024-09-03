#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#include "Help.h"

void ReadConfigFile(const char *configFileName);

int main(const int argc, char **argv) {
    //TODO: build a factory
    if (argc == 1) {
        std::cout << "Use -h option to see help.\n";
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0) {
        Help::Display();
    } else if (strcmp(argv[1], "-c") == 0) {
        ReadConfigFile(argv[2]);
    } else {
        std::cerr << "Unknown argument: \"" << argv[1] << "\"\n";
        return 3;
    }
    return 0;
}

void GetNextWord(std::string &line, std::string &res) {
    std::string word;
    size_t spaceIdx = line.find(' ');
    if (spaceIdx == std::string::npos) {
        res = "";
        return;
    }
    if (spaceIdx == 0) { res = "..."; }
    res = line.substr(0, spaceIdx - 1);
    line = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
}

void GetWords(std::string &line, std::vector<std::string> &res) {
    size_t spaceIdx;
    while ((spaceIdx = line.find(' ')) != std::string::npos) {
        if (!spaceIdx) {
            line = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
            continue;
        }
        res.push_back(line.substr(0, spaceIdx));
        line = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
    }
    //no spaces were found; adding last word in line to array
    if (!line.empty()) {
        res.push_back(line);
    }
}

void GetConverter(const std::vector<std::string> &words) {
    if (words.empty()) {
        return;
    }
    if (words[0] == "mute") {
        std::cout << "This is a mute converter.\n";
        return;
    }
    if (words[0] == "mix") {
        std::cout << "This is a mix converter.\n";
        return;
    }
    if (words[0] == "custom") {
        std::cout << "This is a custom converter.\n";
        return;
    }
    std::cout << "\"" << words[0] << "\" is not a converter.\n";
}

void ReadConfigFile(const char *configFileName) {
    std::ifstream inFile(configFileName);
    if (inFile.fail()) {
        std::cerr << "File \"" << configFileName << "\" doesn't exist.\n";
        return;
    }
    if (!inFile.is_open()) {
        std::cerr << "Couldn't open file \"" << configFileName << "\'.\n";
        return;
    }
    std::string line;
    std::vector<std::string> words;
    while (std::getline(inFile, line)) {
        if (line[0] == '#') {
            continue;
        }
        words.clear();
        GetWords(line, words);
        GetConverter(words);
    }
}
