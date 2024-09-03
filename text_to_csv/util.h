#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

enum ERRORS_CODES
{
    NOT_FIRST = -1,
    NOT_LAST = -2
};

int CheckFiles(int N, const std::string &input, const std::string &output);

int ReadInput(std::wstring &line, std::map<std::wstring, int> &words, const std::string &fileName, int &countWords);

std::vector<std::pair<std::wstring, int>> SortWords(std::map<std::wstring, int> &map);

void PrintVector(const std::vector<std::pair<std::wstring, int>>& vec, const std::string& fileName, int count);