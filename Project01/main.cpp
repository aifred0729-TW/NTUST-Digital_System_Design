#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>

#define PADDING "    "

typedef struct {
	unsigned int element;
	unsigned int loadCount;
	std::vector<std::pair<std::string, int>> PLA;
} PLAdata;

typedef struct node {
	int id;
	int index;
	bool value;
	bool terminal;
	std::vector<struct node*> childNode;
} node;

node* zeroNode = new node();
node* oneNode = new node();

void readFile(PLAdata& data);
void initialize(PLAdata& data);
node* generateTerminalNode(node* lastNode);
node* generateBDD(PLAdata data);
void generateDOTfile(PLAdata data, node* root, std::string dotName);
void applyPLAvalue(node* currentNode, std::string tree);

int main() {
	using namespace std;
	PLAdata data;

	initialize(data);
	
	node* root = generateBDD(data);
	generateDOTfile(data, root, "meow");
	for (unsigned int i = 0; i < data.PLA.size(); i++) {
		applyPLAvalue(root, data.PLA[i].first);
	}

	return 0;
}

void readFile(PLAdata &data) {
	using namespace std;

	ifstream fp;
	string loadS, tmp;
	int loadI;

	fp.open("test.txt");

	fp >> tmp >> data.element;
	fp >> tmp >> data.loadCount;
	for (unsigned int i = 0; i < data.element+1; i++) fp >> tmp;
	fp >> tmp >> tmp;
	fp >> tmp >> data.loadCount;
	for (unsigned int i = 0; i < data.loadCount; i++) {
		fp >> loadS >> loadI;
		data.PLA.push_back(pair<string, int>(loadS, loadI));
	}
	fp >> tmp;

	data.element++;

	return;
}

void initialize(PLAdata& data) {
	readFile(data);

	zeroNode->id = 0;
	zeroNode->value = false;
	zeroNode->terminal = true;
	zeroNode->index = data.element;

	oneNode->id = data.element * data.element;
	oneNode->value = true;
	oneNode->terminal = true;
	oneNode->index = data.element;

	return;
}

node* generateTerminalNode(node* lastNode) {

	lastNode->childNode.push_back(zeroNode);
	lastNode->childNode.push_back(oneNode);

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
		endPointNodes[i] = generateTerminalNode(endPointNodes[i]);
	}
	
	return root;
}

void applyPLAvalue(node* currentNode, std::string tree) {
	if (tree.size() == 1) {
		switch (tree.c_str()[0]) {
		case '-':
			currentNode->childNode[0] = oneNode;
			currentNode->childNode[1] = oneNode;
			return;
		case '0':
			currentNode->childNode[0] = oneNode;
			return;
		case '1':
			currentNode->childNode[1] = oneNode;
			return;
		default:
			return;
		}
	}

	char nodeCondition = tree[0];
	tree.erase(tree.begin(), tree.begin() + 1);

	switch (nodeCondition) {
	case '-':
		applyPLAvalue(currentNode->childNode[0], tree);
		applyPLAvalue(currentNode->childNode[1], tree);
		break;
	case '0':
		applyPLAvalue(currentNode->childNode[0], tree);
		break;
	case '1':
		applyPLAvalue(currentNode->childNode[1], tree);
		break;
	default:
		break;
	}

	return;
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
