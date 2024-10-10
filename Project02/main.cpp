#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

#define PADDING "    "
#define SPLITLINE "\n================================================\n\n"
#define HELP "\n\
- Project 2 by Red / B11215013\n\
\n\
Usage : Project2.exe {PLA file} {output file}\n\
\n"


std::string PLA_FILE = "";
std::string OUT_FILE = "";

typedef struct {
    unsigned int element;
    unsigned int loadCount;
    std::vector<std::pair<std::string, char>> PLA;
} PLAdata;

class term {
public:
    std::string booleanFunc = "";
    std::vector<int> mN;
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

    bool isInIndex(int index) {
        for (unsigned int i = 0; i < mN.size(); i++) {
            if (mN[i] == index) return true;
        }
        return false;
    }

    bool operator<(const term& otherTerm) const {
        return booleanFunc < otherTerm.booleanFunc;
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

    for (unsigned int i = 0; i < initTerm.size()-1; i++) {
        if (initTerm[i].booleanFunc == initTerm[i + 1].booleanFunc) initTerm.erase(initTerm.begin() + i);
    }

    return initTerm;
}

std::vector<int> getDontCareTerms(std::vector<term> bf) {
    using namespace std;

    vector<int> dontCareIndex;

    for (unsigned int i = 0; i < bf.size(); i++) {
        if (bf[i].isDontCare) dontCareIndex.push_back(bf[i].mN[0]);
    }

    return dontCareIndex;
}

std::vector<int> getNormalTermIndex(std::vector<term> bf) {
    std::vector<int> index;
    for (unsigned int i = 0; i < bf.size(); i++) {
        if (!bf[i].isDontCare) index.push_back(bf[i].mN[0]);
    }
    return index;
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

bool checkIsDontCareIndex(int index, std::vector<int> dontCareIndex) {
    for (unsigned int i = 0; i < dontCareIndex.size(); i++) if (index == dontCareIndex[i]) return true;
    return false;
}

std::vector<term> mergeBooleanFunctions(std::vector<term> initBF, std::vector<int> dontCareIndex) {
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
                    for (unsigned int a = 0; a < groupedBF[i][j].mN.size(); a++) {
                        if (checkIsDontCareIndex(groupedBF[i][j].mN[a], dontCareIndex)) continue;
                        primeImplicant.push_back(groupedBF[i][j]);
                        break;
                    }
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

std::vector<term> getEssentialPrimeImplicant(std::vector<int> index, std::vector<term> PI) {
    using namespace std;

    vector<term> EPI;
    vector<vector<int>> EPItable;
    EPItable.resize(index.size());


    for (unsigned int i = 0; i < index.size(); i++) {
        for (unsigned int j = 0; j < PI.size(); j++) {
            if (PI[j].isInIndex(index[i])) EPItable[i].push_back(j);
        }
    }

    for (unsigned int i = 0; i < EPItable.size(); i++) {
        if (EPItable[i].size() == 1) EPI.push_back(PI[EPItable[i][0]]);
    }

    return EPI;
}

std::vector<int> getLastTermIndex(std::vector<int> termIndexs, std::vector<term> EPI) {

    for (unsigned int i = 0; i < termIndexs.size(); i++) {
        for (unsigned int j = 0; j < EPI.size(); j++) {
            if (EPI[j].isInIndex(termIndexs[i])) {
                termIndexs.erase(termIndexs.begin() + i);
                i--;
                break;
            }
        }
    }

    return termIndexs;
}

std::vector<term> getLastPrimeImplicant(std::vector<term> PI, std::vector<term> EPI) {

    for (unsigned int i = 0; i < PI.size(); i++) {
        for (unsigned int j = 0; j < EPI.size(); j++) {
            if (PI[i].booleanFunc == EPI[j].booleanFunc) PI.erase(PI.begin() + i);
        }
    }

    return PI;
}


std::vector<term> runPetrickMethod(std::vector<term> EPI, std::vector<term> lastPI, std::vector<int> lastTermIndex) {
    using namespace std;

    // Polynomial()[] -> +[] -> X[]
    vector<vector<vector<term>>> polynomial;
    polynomial.resize(lastTermIndex.size());

    for (unsigned int i = 0; i < lastPI.size(); i++) {
        for (unsigned int j = 0; j < lastTermIndex.size(); j++) {
            if (lastPI[i].isInIndex(lastTermIndex[j])) polynomial[j].push_back({ {lastPI[i]} });
        }
    }

    vector<vector<term>> stagedPoly;
    vector<term> tmp;

    for (unsigned int i = 0; i < polynomial.size(); i++) {
        for (unsigned int j = 0; j < polynomial[0].size(); j++) {
            for (unsigned int k = 0; k < polynomial[1].size(); k++) {
                for (unsigned int a = 0; a < polynomial[0][j].size(); a++) tmp.push_back(polynomial[0][j][a]);
                tmp.push_back(polynomial[1][k][0]);
                stagedPoly.push_back(tmp);
                tmp.clear();
            }
        }
        polynomial.erase(polynomial.begin() + 1);
        polynomial[0] = stagedPoly;
        stagedPoly.clear();
    }

    vector<vector<term>> expanedPolys = polynomial[0];

    for (unsigned int i = 0; i < expanedPolys.size(); i++) {
        for (unsigned int j = 0; j < expanedPolys[i].size(); j++) {
            for (unsigned int k = j+1; k < expanedPolys[i].size(); k++) {
                if (expanedPolys[i][j].booleanFunc == expanedPolys[i][k].booleanFunc) expanedPolys[i].erase(expanedPolys[i].begin() + k);
            }
        }
        sort(expanedPolys[i].begin(), expanedPolys[i].end());
    }
    sort(expanedPolys.begin(), expanedPolys.end());
    
    for (unsigned int i = 0; i < expanedPolys.size()-1; i++) {
        if (expanedPolys[i].size() == expanedPolys[i + 1].size()) {
            int sameCount = 0;
            for (unsigned int j = 0; j < expanedPolys[i].size(); j++) {
                if (expanedPolys[i][j].booleanFunc == expanedPolys[i + 1][j].booleanFunc) sameCount++;
            }
            if (sameCount == expanedPolys[i].size()) {
                expanedPolys.erase(expanedPolys.begin() + i);
                i--;
            }
        }
    }

    for (unsigned int i = expanedPolys.size() - 1; i > 0; i--) {
        for (unsigned int j = 0; j < i; j++) {
            if (expanedPolys[j].size() > expanedPolys[j + 1].size()) std::swap(expanedPolys[j], expanedPolys[j + 1]);
        }
    }

    vector<term> result;

    for (unsigned int i = 0; i < EPI.size(); i++) {
        result.push_back(EPI[i]);
    }
    for (unsigned int i = 0; i < expanedPolys[0].size(); i++) {
        result.push_back(expanedPolys[0][i]);
    }

    return result;
}

std::string buildResultBuffer(std::vector<term> result) {

    std::string buffer = "";

    buffer += ".i ";
    buffer += std::to_string(result.size());
    buffer += '\n';
    buffer += ".o 1\n";
    buffer += ".lib";

    for (unsigned int i = 0; i < result[0].booleanFunc.size(); i++) {
        buffer += " ";
        buffer += 'a' + i;
    }

    buffer += '\n';
    buffer += ".ob f\n";
    buffer += ".p ";
    buffer += std::to_string(result.size());
    buffer += '\n';

    for (unsigned int i = 0; i < result.size(); i++) {
        buffer += result[i].booleanFunc;
        buffer += " 1\n";
    }

    buffer += ".e\n";

    return buffer;
}

void writeFile(std::string buffer, std::string filename) {
    std::ofstream outFile("output.pla");
    outFile << buffer;
    outFile.close();
    return;
}

int main(int argc, char* argv[]) {
    using namespace std;

    if (argc < 3) { cout << HELP; return 1; }

    PLAdata data;

    PLA_FILE = argv[1];
    OUT_FILE = argv[2];

    readFile(data);

    vector<term> initTerm = expandBooleanFunction(data);
    vector<int> dontCareIndex = getDontCareTerms(initTerm);
    vector<int> normalTermIndex = getNormalTermIndex(initTerm);
    vector<term> PI = mergeBooleanFunctions(initTerm, dontCareIndex);
    vector<term> EPI = getEssentialPrimeImplicant(normalTermIndex, PI);
    vector<int> lastTermIndex = getLastTermIndex(normalTermIndex, EPI);
    vector<term> lastPI = getLastPrimeImplicant(PI, EPI);
    vector<term> result = runPetrickMethod(EPI, lastPI, lastTermIndex);

    writeFile(buildResultBuffer(result), OUT_FILE);

    return 0;
}