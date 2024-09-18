#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>

#define PADDING "    "

typedef struct {
	unsigned int element;
	unsigned int loadCount;
	std::vector<std::pair<std::string, char>> PLA;
} PLAdata;

typedef struct node {
	int id;
	int index;
	bool value;
	bool terminal;
	std::vector<struct node*> childNode;
} node;

void readFile(PLAdata& data);
node* generateTerminalNode(int element, node* lastNode);
node* generateBDD(PLAdata data);
void generateDOTfile(PLAdata data, node* root, std::string dotName);

int main() {
	using namespace std;
	PLAdata data;
	
	readFile(data);
	cout << data.element << endl << data.loadCount << endl;
	
	for (unsigned int i = 0; i < data.loadCount; i++) {
		cout << data.PLA[i].first << ' ' << data.PLA[i].second << endl;
	}
	node* root = generateBDD(data);
	generateDOTfile(data, root, "meow");

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

	data.element++;

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
	int id = 1;

	root->id = id;
	root->index = 1;

	endPointNodes.push_back(root);

	for (unsigned int i = 2; i < data.element; i++) {
		for (unsigned int j = 0; j < endPointNodes.size(); j++) {
			for (unsigned int k = 0; k < 2; k++) {
				node* newNode = new node();

				id++;
				newNode->id = id;
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

void getAllLinkListPointer(std::set<node*>& nodes, node* root) {
	if (root->childNode.size() == 0) {
		nodes.insert(root);
		return;
	}
	nodes.insert(root);
	getAllLinkListPointer(nodes, root->childNode[0]);
	getAllLinkListPointer(nodes, root->childNode[1]);
	return;
}

void generateDOTfile(PLAdata data, node* root, std::string dotName) {
	using namespace std;

	auto printLayout = [&](string id) -> void {
		cout << PADDING << "{rank=same" << id << "}" << endl;
		};

	auto printElement = [&](int id, char element) -> void {
		cout << PADDING << id << " [label=\"" << element << "\"]" << endl;
		};

	auto printLine = [&](int src, int dst, bool zero) -> void {
		cout << PADDING;
		if (zero) cout << src << " -> " << dst << "[label=\"0\", style=dotted]" << endl;
		else cout << src << " -> " << dst << "[label=\"1\", style=solid]" << endl;
		};

	cout << "digraph " << dotName << " {" << endl;

	string id;
	set<node*> nodes;
	getAllLinkListPointer(nodes, root);

	// Output Layout
	for (unsigned int i = 1; i < data.element; i++) {
		for (auto& s : nodes) if (s->index == i) id = id + " " + to_string(s->id);
		printLayout(id);
		id = "";
	} cout << endl;

	// Output Element
	for (unsigned int i = 1; i < data.element; i++) for (auto& s : nodes) if (s->index == i) printElement(s->id, 96 + s->index);
	cout << PADDING << "0 [label=0, shape=box]" << endl;
	cout << PADDING << data.element * data.element << " [label=1, shape=box]" << endl << endl;

	// Output Line
	for (unsigned int i = 1; i < data.element-1; i++) {
		for (auto& s : nodes) {
			if (s->index == i) {
				printLine(s->id, s->childNode[0]->id, true);
				printLine(s->id, s->childNode[1]->id, false);
			}
		}
	}
	for (auto& s : nodes) {
		if (s->index == data.element - 1) {
			printLine(s->id, 0, true);
			printLine(s->id, data.element * data.element, false);
		}
	}

	cout << "}" << endl;

	return;
}
