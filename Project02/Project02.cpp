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

typedef std::vector<std::pair<std::string, int>> expanedBF;

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

void expendPLADontCare(expanedBF& data, std::string booleanFunction, int index) {

    if (index == booleanFunction.size()) {
        data.push_back(std::pair<std::string, int>(booleanFunction, -1));
        return;
    }

    switch (booleanFunction[index]) {
    case '-':
        booleanFunction[index] = '0';
        expendPLADontCare(data, booleanFunction, index+1);
        booleanFunction[index] = '1';
        expendPLADontCare(data, booleanFunction, index+1);
        return;
    default:
        expendPLADontCare(data, booleanFunction, index+1);
        return;
    }

    return;
}


void expandBooleanFunction(PLAdata data, expanedBF& initializeBF) {
    for (unsigned int i = 0; i < data.loadCount; i++) if (data.PLA[i].second == '1') expendPLADontCare(initializeBF, data.PLA[i].first, 0);
    return;
}

int main() {
    using namespace std;

    PLAdata data;
    PLA_FILE = "test.pla";

    readFile(data);

    expanedBF initializeBF;
    expandBooleanFunction(data, initializeBF);

    return 0;
}