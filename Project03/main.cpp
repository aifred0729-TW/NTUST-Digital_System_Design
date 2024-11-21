#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#include <map>

typedef struct {
    bool input;
    bool output;
    int current;
    int next;
} status;

class kiss {
public:
    int inputCount = 0;
    int outputCount = 0;
    int lineCount = 0;
    int statusCount = 0;
    int initialStatus = 0;

    std::vector<status> statuses;
    std::vector<
        std::vector<
        std::vector<
        std::pair<int, int>>>
    > statusMatrix;

    int nameAllocation = 0;
    std::map<std::string, std::string> nameMap;

    char existName(std::string name) {
        for (auto s = nameMap.begin(); s != nameMap.end(); ++s) {
            if (s->second == name) return s->first.c_str()[0];
        }
        return NULL;
    }

    void nameAlloc(int& nameIndex, std::string name) {
        char result = existName(name);
        if (result == NULL) {
            nameIndex = nameAllocation;
            nameMap.insert(std::pair<std::string, std::string>(std::string(1, ('a' + nameAllocation)), name));
            nameAllocation++;
            return;
        }
        nameIndex = result - 'a';
        return;
    }

    void insertStatus(std::string input, std::string output, std::string current, std::string next) {
        status tmpStatus;
        tmpStatus.input = (bool)(input[0] - '0');
        tmpStatus.output = (bool)(output[0] - '0');
        
        tmpStatus.current = nameAllocation;
        nameAlloc(tmpStatus.current, current);

        tmpStatus.next = nameAllocation;
        nameAlloc(tmpStatus.next, next);

        statuses.push_back(tmpStatus);

        return;
    }

    std::string getNameByIndex(int nameIndex) {
        return nameMap[std::string(1, nameIndex + 'a')];
    }
};

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) tokens.push_back(token);

    return tokens;
}

void processStateLine(std::string line, std::vector<std::string> &strArr) {
    std::stringstream ss(line);
    std::string tmpString = "";
    
    ss >> tmpString;
    strArr.push_back(tmpString);
    ss >> tmpString;
    strArr.push_back(tmpString);
    ss >> tmpString;
    strArr.push_back(tmpString);
    ss >> tmpString;
    strArr.push_back(tmpString);

}

kiss readFile(std::string filename) {
    kiss k;
    std::ifstream fp;
    std::vector<std::string> tmpArray;
    std::string line, tmpString;
    int tmpNumber = 0;
    char tmpChar = '\0';

    fp.open(filename);

    getline(fp, line); // File Header
    getline(fp, line); tmpArray = split(line, ' '); k.inputCount = std::stoi(tmpArray[1]);     // Input Count
    getline(fp, line); tmpArray = split(line, ' '); k.outputCount = std::stoi(tmpArray[1]);    // Output Count
    getline(fp, line); tmpArray = split(line, ' '); k.lineCount = std::stoi(tmpArray[1]);      // Line Count
    getline(fp, line); tmpArray = split(line, ' '); k.statusCount = std::stoi(tmpArray[1]);    // Status Count
    getline(fp, line); tmpArray = split(line, ' '); k.nameAlloc(k.initialStatus, tmpArray[1]); // Initial Status

    for (size_t i = 0; i < k.lineCount; i++) {
        status tmpStatus;
        getline(fp, line);
        std::vector<std::string> tmpArray;
        processStateLine(line, tmpArray);
        k.insertStatus(tmpArray[0], tmpArray[3], tmpArray[1], tmpArray[2]);
    }

    getline(fp, line); // File EndHeader

    return k;
}

int getStatusIndex(int current, int input, std::vector<status> statuses) {
    for (size_t i = 0; i < statuses.size(); i++) {
        if (statuses[i].current == current && statuses[i].input == (bool)input) return (int)i;
    }
    return -1;
}

bool checkBadPairOutput(int current, int compare, int input, std::vector<status> statuses) {
    if (statuses[getStatusIndex(current, input, statuses)].output !=
        statuses[getStatusIndex(compare, input, statuses)].output) return true;
    return false;
}

std::vector<std::pair<int, int>> generateElement(int current, int compare, std::vector<status> statuses) {
    std::vector<std::pair<int, int>> element;

    for (size_t i = 0; i < 2; i++) {
        if (checkBadPairOutput(current, compare, i, statuses)) {
            element.push_back(std::pair<int, int>(-1, -1));
            continue;
        }

        std::pair<int, int> tmpPair;

        tmpPair.first = statuses[getStatusIndex(compare, i, statuses)].next;
        tmpPair.second = statuses[getStatusIndex(current, i, statuses)].next;
        if (tmpPair.first < tmpPair.second) std::swap(tmpPair.first, tmpPair.second);
        element.push_back(tmpPair);
    }

    return element;
}

bool checkIsBadNode(std::vector<std::pair<int, int>> element) {
    for (size_t i = 0; i < element.size(); i++) {
        if (element[i].first == -1) return true;
    }
    return false;
}

void generateStateMatrix(kiss& k) {
    k.statusMatrix.resize(k.statusCount);

    for (size_t i = 1; i < k.statusCount; i++) {
        for (size_t j = 0; j < i; j++) {
            std::vector<std::pair<int, int>> element = generateElement(i, j, k.statuses);
            if (checkIsBadNode(element)) k.statusMatrix[i].push_back(std::vector<std::pair<int, int>>());
            else k.statusMatrix[i].push_back(element);
        }
    }

    std::cout << "[+] Statuses Matrix has been Genereated.\n";

    return;
}

bool checkIsIncludeBadNode(int _row, int _col, std::vector<std::vector<std::vector<std::pair<int, int>>>> statusMatrix) {
    for (size_t i = 0; i < statusMatrix[_row][_col].size(); i++) {
        int row = statusMatrix[_row][_col][i].first;
        int col = statusMatrix[_row][_col][i].second;
        if (row >= statusMatrix.size()) continue;
        if (col >= statusMatrix[row].size()) continue;
        if (statusMatrix[row][col].size() == 0) {
            std::cout << "[!] Check Node " << std::string(1, (_row + 'A')) << "-" << std::string(1, (_col + 'A'))
                << " on Bad Node " << std::string(1, (row + 'A')) << "-" << std::string(1, (col + 'A')) << std::endl;
            return true;
        }
    }
    return false;
}

void printMatrix(kiss k) {

    std::cout << "\n======================================================= Statuses Matrix =======================================================\n\n";

    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        std::cout << ">    "; if (i < 10) std::cout << ' ';
        std::cout << i << " - " << std::string(1, (i + 'A')) << " | ";
        for (size_t j = 0; j < i; j++) {
            if (k.statusMatrix[i][j].size() == 0) {
                std::cout << "  NULL    / / ";
                continue;
            }
            for (size_t a = 0; a < k.statusMatrix[i][j].size(); a++) {
                std::cout << std::string(1, (k.statusMatrix[i][j][a].first + 'A')) << '-' << std::string(1, (k.statusMatrix[i][j][a].second + 'A')) << " / ";
            }
            std::cout << "/ ";
        }
        putchar('\n');
    }


    std::cout << ">    -------+---------------------------------------------------------------------------------------------------\n";
    std::cout << ">           |";

    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        std::cout << "    " << i << " - " << std::string(1, (i + 'A')) << "    |";
    }
    putchar('\n');

    std::cout << "\n======================================================= Statuses Matrix =======================================================\n\n";
    return;
}

void optimizationMatrix(kiss& k, int layer) {

    std::cout << "[+] Starting Optimization Statuses Matrix\n";

    bool optimizationRound = false;

    printMatrix(k);
    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        for (size_t j = 0; j < i; j++) {
            if (checkIsIncludeBadNode(i, j, k.statusMatrix)) {
                k.statusMatrix[i][j].erase(k.statusMatrix[i][j].begin(), k.statusMatrix[i][j].end());
                optimizationRound = true;
            }
        }
    }

    std::cout << "[=] Finish optimization matrix on layer " << layer << std::endl;
    if (optimizationRound) optimizationMatrix(k, layer+1);

    return;
}

void replaceStatus(int row, int col, int remove, int replace, kiss &k) {
    for (size_t i = 0; i < k.statusMatrix[row][col].size(); i++) {
        if (k.statusMatrix[row][col][i].first == remove) k.statusMatrix[row][col][i].first = replace;
        if (k.statusMatrix[row][col][i].second == remove) k.statusMatrix[row][col][i].second = replace;
    }
    return;
}

void simplifyProcess(int remove, int replace, kiss& k) {
    int removeIndex = 0;

    std::cout << "[+] Remove Status - " << std::string(1, (remove + 'a')) << std::endl;

    for (size_t i = 0; i < 2; i++) {
        removeIndex = getStatusIndex(remove, i, k.statuses);
        k.statuses.erase(k.statuses.begin() + removeIndex);
    }

    for (size_t i = 0; i < k.statuses.size(); i++) {
        if (k.statuses[i].next == remove) k.statuses[i].next = replace;
    }
    
    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        for (size_t j = 1; j < i; j++) {
            replaceStatus(i, j, remove, replace, k);
        }
    }

    for (size_t i = 0; i < k.statusMatrix[remove].size(); i++) {
        k.statusMatrix[remove][i].erase(k.statusMatrix[remove][i].begin(), k.statusMatrix[remove][i].end());
    }

    k.nameMap.erase(std::string(1, ('a' + remove)));

    printMatrix(k);

    return;
}

void simplifyStatus(kiss& k) {


    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        for (size_t j = 0; j < i; j++) {
            if (k.statusMatrix[i][j].size() != 0) {
                simplifyProcess(i, j, k);
                k.statusCount--;
            }
        }
    }

    k.lineCount = k.statuses.size();

    return;
}

std::string generatekissLine(kiss& k, int index) {
    std::string buffer = "";

    buffer += (char)(k.statuses[index].input + '0'); buffer += ' ';
    buffer += k.getNameByIndex(k.statuses[index].current); buffer += ' ';
    buffer += k.getNameByIndex(k.statuses[index].next); buffer += ' ';
    buffer += (char)(k.statuses[index].output + '0'); buffer += '\n';

    return buffer;
}

std::string generatekissResult(kiss &k) {
    std::string buffer = "";

    buffer += ".start_kiss\n";
    buffer += ".i " + std::to_string(k.inputCount) + '\n';
    buffer += ".o " + std::to_string(k.outputCount) + '\n';
    buffer += ".p " + std::to_string(k.lineCount) + '\n';
    buffer += ".s " + std::to_string(k.statusCount) + '\n';
    buffer += ".r " + k.getNameByIndex(k.initialStatus) + '\n';
    
    for (size_t i = 0; i < k.lineCount; i++) {
        buffer += generatekissLine(k, i);
    }

    buffer += ".end_kiss\n";

    return buffer;
}

std::string generateDotInitLine(std::string s) {
    std::string buffer = "";

    buffer += "    ";
    buffer += s;
    buffer += " [label=\"";
    buffer += s;
    buffer += "\"];\n";

    return buffer;
}

std::string generateDotStateLine(kiss& k) {
    std::string buffer = "";

    std::string current = k.getNameByIndex(k.statuses[0].current);
    std::string next = k.getNameByIndex(k.statuses[0].next);

    int zero = getStatusIndex(k.statuses[0].current, 0, k.statuses);
    int one = getStatusIndex(k.statuses[0].current, 1, k.statuses);

    buffer += "    ";
    buffer += current;
    buffer += " -> ";
    buffer += next;
    buffer += " [label=\"";

    if (zero != -1 && one != -1 && k.statuses[zero].next == k.statuses[one].next) {
        buffer += std::to_string(k.statuses[zero].input);
        buffer += '/';
        buffer += std::to_string(k.statuses[zero].output);
        buffer += ",";
        buffer += std::to_string(k.statuses[one].input);
        buffer += '/';
        buffer += std::to_string(k.statuses[one].output);
        k.statuses.erase(k.statuses.begin() + zero);
        one = getStatusIndex(k.statuses[0].current, 1, k.statuses);
        k.statuses.erase(k.statuses.begin() + one);
    }
    else {
        buffer += std::to_string(k.statuses[0].input);
        buffer += "/";
        buffer += std::to_string(k.statuses[0].output);
        k.statuses.erase(k.statuses.begin());
    }

    buffer += "\"];\n";

    return buffer;
}

std::string generateDotResult(kiss& k) {
    std::string buffer = "";
    std::string pattern = "    ";
    std::vector<std::string> names;

    buffer += "digraph STG {\n";
    buffer += pattern; buffer += "rankdir=LR;\n\n";
    buffer += pattern; buffer += "INIT [shape=point];\n";

    for (auto &s : k.nameMap) {
        names.push_back(s.second);
    }

    std::sort(names.begin(), names.end());

    for (auto& s : names) {
        buffer += generateDotInitLine(s);
    }


    buffer += '\n';
    buffer += pattern; buffer += "INIT -> "; buffer += k.getNameByIndex(k.initialStatus); buffer += ";\n";
    
    while(k.statuses.size() != 0) {
        buffer += generateDotStateLine(k);
    }

    buffer += "}\n";

    return buffer;
}

void writeFile(kiss& k, std::string filename) {
    std::ofstream fp(filename);
    std::string buffer = generatekissResult(k);
    fp << buffer;
    fp.close();

    buffer = generateDotResult(k);
    std::cout << buffer;

    return;
}

void optimizationMatrixDBG(kiss& k) {
    using namespace std;

    int optimizationCount = 0;
    std::set<std::string> stat;

    for (size_t i = 0; i < k.statusMatrix.size(); i++) {
        for (size_t j = 0; j < k.statusMatrix[i].size(); j++) {
            if (k.statusMatrix[i][j].size() != 0) {
                for (size_t a = 0; a < k.statusMatrix[i][j].size(); a++) {
                    stat.insert(std::string(1, (k.statusMatrix[i][j][a].first + 'a')));
                    stat.insert(std::string(1, (k.statusMatrix[i][j][a].second + 'a')));
                }
                optimizationCount++;
            }
        }
    }

    cout << "[+] Total Optimizable Block Count : " << optimizationCount << endl;
    cout << "[>] Optimizable Status : " << endl;
    for (const auto &s : stat) {
        cout << "    -> " << s << endl;
    }
    
    return;
}

void DebugMessage(int opcode, kiss k) {
    using namespace std;

    switch (opcode) {
    case 1:
        break;
    case 2:
        optimizationMatrixDBG(k);
        printMatrix(k);
        break;
    case 3:
        break;
    }

    return;
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cout << "Usage : " << argv[0] << " {input.kiss} {output name}" << std::endl;
        std::cout << "  output name \\" << std::endl;
        std::cout << "               | - output.kiss" << std::endl;
        std::cout << "               | - output.dot" << std::endl;
        return 1;
    }
    
    kiss k = readFile("case5.kiss");
    generateStateMatrix(k);
    printMatrix(k);
    optimizationMatrix(k, 1);
    DebugMessage(2, k);
    simplifyStatus(k);
    writeFile(k, "result.kiss");
    return 0;
}