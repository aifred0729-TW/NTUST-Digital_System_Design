#include <iostream>
#include <string>
#include <fstream>
#include <vector>

typedef struct {
	int element;
	int loadCount;
	std::vector<std::pair<std::string, char>> PLA;
} PLAdata;

typedef struct node {
	int index;
	bool value;
	bool terminal;
	std::vector<struct node*> childNode;
} node;

void readFile(PLAdata& data);
node* generateTerminalNode(int element, node* lastNode);
node* generateBDD(PLAdata data);

int main() {
	using namespace std;
	PLAdata data;
	
	readFile(data);
	cout << data.element << endl << data.loadCount << endl;
	
	for (unsigned int i = 0; i < data.loadCount; i++) {
		cout << data.PLA[i].first << ' ' << data.PLA[i].second << endl;
	}
	node* root = generateBDD(data);

	return 0;
}

void readFile(PLAdata &data) {
	using namespace std;

	ifstream fp;
	string loadS, tmp;
	char loadC;

	fp.open("test.txt");

	fp >> tmp >> data.element;
	fp >> tmp >> data.loadCount;
	for (unsigned int i = 0; i < data.element+1; i++) fp >> tmp;
	fp >> tmp >> tmp;
	fp >> tmp >> data.loadCount;
	for (unsigned int i = 0; i < data.loadCount; i++) {
		fp >> loadS >> loadC;
		data.PLA.push_back(pair<string, char>(loadS, loadC));
	}
	fp >> tmp;

	return;
}

node* generateTerminalNode(int element, node* lastNode) {

	node* valueNodeL = new node();
	valueNodeL->index = element;
	valueNodeL->terminal = true;
	valueNodeL->value = false;

	node* valueNodeR = new node();
	valueNodeR->index = element;
	valueNodeR->terminal = true;
	valueNodeR->value = true;

	lastNode->childNode.push_back(valueNodeL);
	lastNode->childNode.push_back(valueNodeR);

	return lastNode;
}

node* generateBDD(PLAdata data) {
	node* root = new node();
	std::vector<node*> endPointNodes, tmpNodes;

	endPointNodes.push_back(root);

	for (unsigned int i = 1; i < data.element; i++) {
		for (unsigned int j = 0; j < endPointNodes.size(); j++) {
			for (unsigned int k = 0; k < 2; k++) {
				node* newNode = new node();

				newNode->index = i;
				newNode->value = NULL;
				newNode->terminal = false;

				tmpNodes.push_back(newNode);
				endPointNodes[j]->childNode.push_back(newNode);
			}
		}
		endPointNodes = tmpNodes;
		tmpNodes.clear();
	}

	for (unsigned int i = 0; i < endPointNodes.size(); i++) {
		endPointNodes[i] = generateTerminalNode(data.element, endPointNodes[i]);
	}
	
	return root;
}

void applyPLAvalue(node* root, PLAdata data) {
	for (unsigned int i = 0; i < data.loadCount; i++) {
		return;
	}
}