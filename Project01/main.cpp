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
	struct node* lNode;
	struct node* rNode;
} node;

void readFile(PLAdata& data);
node* generateNewNode(int element, node* oldNode);
node* generateBDD(PLAdata data);

int main() {
	using namespace std;
	PLAdata data;
	
	readFile(data);
	cout << data.element << endl << data.loadCount << endl;
	
	for (unsigned int i = 0; i < data.loadCount; i++) {
		cout << data.PLA[i].first << ' ' << data.PLA[i].second << endl;
	}

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


node* generateLastNode(int element, node* lastNode) {

	node* valueNodeL = (node*)malloc(sizeof(node));
	valueNodeL->index = 0;
	valueNodeL->lNode = NULL;
	valueNodeL->rNode = NULL;
	valueNodeL->terminal = NULL;
	valueNodeL->value = false;


	node* valueNodeR = (node*)malloc(sizeof(node));
	valueNodeR->index = 0;
	valueNodeR->lNode = NULL;
	valueNodeR->rNode = NULL;
	valueNodeR->terminal = NULL;
	valueNodeR->value = true;

	lastNode->terminal = true;
	lastNode->index = element - 1;
	lastNode->value = NULL;
	lastNode->lNode = valueNodeL;
	lastNode->rNode = valueNodeR;

	return;
}

node* _generateBDD(int element, node* oldNode) {
	if (oldNode->index == element) return generateLastNode(element, oldNode);

	node* newLeft = (node*)malloc(sizeof(node));
	node* newRight = (node*)malloc(sizeof(node));

	newLeft->index = element;
	newRight->index = element;

	oldNode->lNode = newLeft;
	oldNode->rNode = newRight;

	return _generateBDD(element + 1, newLeft);
	
}

node* generateBDD(PLAdata data) {
	node* root = (node*)malloc(sizeof(node));

	

	return root;
}