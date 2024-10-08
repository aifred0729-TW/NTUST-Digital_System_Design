#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

std::string PLA_FILE = "";

typedef struct {
    unsigned int element;
    unsigned int loadCount;
    std::vector<std::pair<std::string, char>> PLA;
} PLAdata;

typedef std::pair<std::string, std::vector<int>> booleanFunc;

void readFile(PLAdata& data) {
    using namespace std;

    ifstream fp;
    string loadS, tmp;
    char loadC;

    fp.open(PLA_FILE);

    fp >> tmp >> data.element;
    fp >> tmp >> data.loadCount;
    for (unsigned int i = 0; i < data.element + 1; i++) fp >> tmp;
    fp >> tmp >> tmp;
    fp >> tmp >> data.loadCount;
    for (unsigned int i = 0; i < data.loadCount; i++) {
        fp >> loadS >> loadC;
        data.PLA.push_back(pair<string, char>(loadS, loadC));
    }
    fp >> tmp;

    data.element++;

    return;
}

void expendPLADontCare(std::vector<booleanFunc>& data, std::string booleanFunction, int index, int isDontCare) {

    if (index == booleanFunction.size()) {
        data.push_back(std::pair<std::string, std::vector<int>>(booleanFunction, { 0, isDontCare }));
        return;
    }

    if (booleanFunction[index] == '-') {
        booleanFunction[index] = '0';
        expendPLADontCare(data, booleanFunction, index + 1, isDontCare);
        booleanFunction[index] = '1';
        expendPLADontCare(data, booleanFunction, index + 1, isDontCare);
    } else {
        expendPLADontCare(data, booleanFunction, index+1, isDontCare);
    }

    return;
}


void expandBooleanFunction(PLAdata data, std::vector<booleanFunc>& initializeBF) {
    for (unsigned int i = 0; i < data.loadCount; i++) {
        if (data.PLA[i].second == '1') expendPLADontCare(initializeBF, data.PLA[i].first, 0, 0);
        if (data.PLA[i].second == '-') expendPLADontCare(initializeBF, data.PLA[i].first, 0, 1);
    }
    for (unsigned int i = 0; i < initializeBF.size(); i++) {
        initializeBF[i].second[0] = std::stoi(initializeBF[i].first, nullptr, 2);
    }
    for (unsigned int i = initializeBF.size() - 1; i > 0; i--) {
        for (unsigned int j = 0; j < i; j++) {
            if (initializeBF[j].second > initializeBF[j + 1].second) std::swap(initializeBF[j], initializeBF[j + 1]);
        }
    }
    return;
}

void groupBooleanFunctions(std::vector<booleanFunc>& bf, std::vector<std::vector<booleanFunc>>& groupedBF) {
    using namespace std;

    int trueCount = 0;
    groupedBF.resize(bf[0].first.size() + 1);

    for (unsigned int i = 0; i < bf.size(); i++) {
        for (unsigned int j = 0; j < bf[i].first.size(); j++) {
            if (bf[i].first[j] == '1') trueCount++;
        }
        groupedBF[trueCount].push_back(bf[i]);
        trueCount = 0;
    }

    return;
}

void mergeBooleanFunctions(std::vector<std::vector<booleanFunc>>& bf) {
    using namespace std;

    for (unsigned int i = 0; i < bf.size(); i++) {

    }

    return;
}

int main() {
    using namespace std;

    PLAdata data;
    PLA_FILE = "test.pla";

    readFile(data);

    vector<booleanFunc> initializeBF;

    expandBooleanFunction(data, initializeBF);

    vector<vector<booleanFunc>> groupedBF;

    groupBooleanFunctions(initializeBF, groupedBF);


    return 0;
}