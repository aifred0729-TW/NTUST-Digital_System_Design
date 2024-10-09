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

class term {
public:
    std::string booleanFunc = "";
    std::vector<int> mN;
    std::vector<int> dontCareIndex;
    bool isDontCare = false;
    bool optimized = false;

    term(std::string _booleanFunc, bool _isDontCare) {
        booleanFunc = _booleanFunc;
        isDontCare = _isDontCare;
    }

    bool isInTerm(term initTerm) {
        if (initTerm.isDontCare) return true;
        for (unsigned int i = 0; i < booleanFunc.size(); i++) {
            if (booleanFunc[i] == '-') continue;
            if (booleanFunc[i] != initTerm.booleanFunc[i]) return false;
        }
        return true;
    }
};

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

void expendPLADontCare(std::vector<term>& data, std::string booleanFunction, int index, bool isDontCare) {

    if (index == booleanFunction.size()) {
        term newTerm(booleanFunction, isDontCare);
        data.push_back(newTerm);
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

std::vector<term> expandBooleanFunction(PLAdata data) {
    std::vector<term> initTerm;

    for (unsigned int i = 0; i < data.loadCount; i++) {
        if (data.PLA[i].second == '1') expendPLADontCare(initTerm, data.PLA[i].first, 0, false);
        if (data.PLA[i].second == '-') expendPLADontCare(initTerm, data.PLA[i].first, 0, true);
    }
    for (unsigned int i = 0; i < initTerm.size(); i++) {
        initTerm[i].mN.push_back(std::stoi(initTerm[i].booleanFunc, nullptr, 2));
    }
    for (unsigned int i = initTerm.size() - 1; i > 0; i--) {
        for (unsigned int j = 0; j < i; j++) {
            if (initTerm[j].mN[0] > initTerm[j + 1].mN[0]) std::swap(initTerm[j], initTerm[j + 1]);
        }
    }
    return initTerm;
}

void groupBooleanFunctions(std::vector<term> bf, std::vector<std::vector<term>>& groupedBF) {
    using namespace std;

    int trueCount = 0;

    for (unsigned int i = 0; i < bf.size(); i++) {
        for (unsigned int j = 0; j < bf[i].booleanFunc.size(); j++) {
            if (bf[i].booleanFunc[j] == '1') trueCount++;
        }
        if (trueCount+1 > groupedBF.size()) groupedBF.resize(trueCount+1);
        groupedBF[trueCount].push_back(bf[i]);
        trueCount = 0;
    }

    return;
}

std::vector<term> mergeBooleanFunctions(std::vector<term> initBF) {
    using namespace std;
    
    vector<term> stageBF;
    vector<term> primeImplicant;
    vector<vector<term>> groupedBF;

    groupBooleanFunctions(initBF, groupedBF);

    while (true) {

        for (unsigned int g = 0; g < groupedBF.size() - 1; g++) {
            for (unsigned int LoG = 0; LoG < groupedBF[g].size(); LoG++) { // Lower Layer
                for (unsigned int HiG = 0; HiG < groupedBF[g + 1].size(); HiG++) { // High Layer

                    //printf("[+] Optimize LoG %d : HiG : %d\n", LoG, HiG);

                    int sameCount = 0;
                    unsigned int index = 0;

                    for (unsigned int i = 0; i < groupedBF[g][LoG].booleanFunc.size(); i++) { // Boolean Function Index
                        //printf("[+] Index %d : Compare L %c H %c\n", i, groupedBF[g][LoG].booleanFunc[i], groupedBF[g + 1][HiG].booleanFunc[i]);
                        if (groupedBF[g][LoG].booleanFunc[i] != groupedBF[g+1][HiG].booleanFunc[i]) {
                            sameCount++;
                            index = i;
                        }
                        if (sameCount > 1) break;
                    }
                    if (sameCount == 1) {
                        groupedBF[g][LoG].optimized = true;
                        groupedBF[g + 1][HiG].optimized = true;

                        term newTerm(groupedBF[g][LoG].booleanFunc, false);
                        newTerm.booleanFunc[index] = '-';
                        for (unsigned int i = 0; i < groupedBF[g][LoG].mN.size(); i++) newTerm.mN.push_back(groupedBF[g][LoG].mN[i]);
                        for (unsigned int i = 0; i < groupedBF[g+1][HiG].mN.size(); i++) newTerm.mN.push_back(groupedBF[g+1][HiG].mN[i]);
                        stageBF.push_back(newTerm);

                        cout << "[+] Merge " << groupedBF[g][LoG].booleanFunc << ":" << groupedBF[g + 1][HiG].booleanFunc << " to " << newTerm.booleanFunc << endl;
                    }
                }
            }
        }

        for (unsigned int i = 0; i < stageBF.size(); i++) {
            for (unsigned int j = i+1; j < stageBF.size(); j++) {
                if (stageBF[i].booleanFunc == stageBF[j].booleanFunc) stageBF.erase(stageBF.begin() + j);
            }
        }

        bool finalRound = true;

        for (unsigned int i = 0; i < groupedBF.size(); i++) {
            for (unsigned int j = 0; j < groupedBF[i].size(); j++) {
                if (groupedBF[i][j].optimized) {
                    groupedBF[i][j].optimized = false;
                    finalRound = false;
                }
                else {
                    primeImplicant.push_back(groupedBF[i][j]);
                }
            }
        }

        cout << "[+] Stored Prime Implicant : " << primeImplicant.size() << endl;

        if (finalRound) return primeImplicant;

        groupedBF.clear();
        groupBooleanFunctions(stageBF, groupedBF);
        stageBF.clear();
    }
}

std::vector<term> getEssentialPrimeImplicant(std::vector<term> initTerm, std::vector<term> PI) {
    using namespace std;

    vector<term> EPI;
    vector<vector<bool>> tableEPI;

    tableEPI.resize(PI.size());

    for (unsigned int i = 0; i < tableEPI.size(); i++) {
        tableEPI[i].resize(initTerm.size());
    }

    for (unsigned int i = 0; i < PI.size(); i++) {
        for (unsigned int j = 0; j < initTerm.size(); i++) {
            for (unsigned int k = 0; k < PI[i].booleanFunc.size(); k++) {
                if (PI[i].isInTerm(initTerm[i])) tableEPI[i][j] = true;
            }
        }
    }

    vector<pair<int, int>> result;

    for (unsigned int i = 0; i < initTerm.size(); i++) {
        int count = 0;
        for (unsigned int j = 0; j < PI.size(); j++) {
            if (tableEPI[i][j]);
        }
    }

    return EPI;
}

int main() {
    using namespace std;

    PLAdata data;
    PLA_FILE = "test.pla";

    readFile(data);

    vector<term> initTerm = expandBooleanFunction(data);
    vector<term> primeImplicant = mergeBooleanFunctions(initTerm);
    printf("%d", primeImplicant.size());
    vector<term> EPI = getEssentialPrimeImplicant(initTerm, primeImplicant);

    return 0;
}