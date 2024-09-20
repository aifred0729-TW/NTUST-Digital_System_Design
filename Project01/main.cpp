#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

#define PADDING "    "
#define SPLITLINE "\n================================================\n\n"

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
	std::vector<std::pair<node*, int>> rootNode;
} node;

void readFile(PLAdata& data);
void initialize(PLAdata& data);
node* generateTerminalNode(node* lastNode);
node* generateBDD(PLAdata data);
void generateDOTfile(PLAdata data, node* root, std::string dotName);
void applyPLAvalue(node* &currentNode, std::string tree);
bool optimizationDoubleReference(node* currentNode);
void optimizationNodes(PLAdata data, node* root);

node* zeroNode = new node();
node* oneNode = new node();
bool verbose = new bool;

int main(int argc, char* argv[]) {
	using namespace std;
	PLAdata data;

	if (argc > 2) if (argv[1] == "-v") verbose = true; 
	else verbose = false;

	if (verbose) cout << "[+] Verbose Mode for Debugging" << endl;

	initialize(data);
	
	node* root = generateBDD(data);

	if (verbose) cout << SPLITLINE << "[+] Apply PLA data to Binary Tree Linklist Nodes\n\n";

	for (unsigned int i = 0; i < data.PLA.size(); i++) {
		if (verbose) cout << "[+] -------- Apply series " << i << " PLA data \"" << data.PLA[i].first << "\" --------\n";
		applyPLAvalue(root, data.PLA[i].first);
	}

	if (verbose) cout << "\n[+] Success Apply PLA data on Binary Tree Linklist Nodes\n";
	if (verbose) cout << SPLITLINE << "[+] Optimization Binary Tree Linklist Nodes\n\n";

	optimizationNodes(data, root);

	if (verbose) cout << SPLITLINE << "[+] Starting generate dot file\n\n";

	generateDOTfile(data, root, "meow");

	if (verbose) cout << SPLITLINE;

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
	lastNode->childNode.push_back(zeroNode);

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
				newNode->rootNode.push_back(std::pair<node*, int>(endPointNodes[j], k));

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

void applyPLAvalue(node* &currentNode, std::string tree) {
	if (tree.size() == 1) {
		if (verbose) printf("[+] Apply TRUE value on node %2d:", currentNode->id);
		switch (tree.c_str()[0]) {
		case '-':
			if (verbose) printf("A\n");
			currentNode->childNode[0] = oneNode;
			currentNode->childNode[1] = oneNode;
			return;
		case '0':
			if (verbose) printf("0\n");
			currentNode->childNode[0] = oneNode;
			return;
		case '1':
			if (verbose) printf("1\n");
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

node* getRootNode(node* currentNode) {
	return currentNode->rootNode[0].first;
}

int getRootBranch(node* currentNode) {
	return currentNode->rootNode[0].second;
}

node* getChildNode(node* currentNode, int branch) {
	return currentNode->childNode[branch];
}

void rebindLinknlist(node* src, node* dst, node* mid, int srcBranch, int type) {
	using namespace std;
	
	if (verbose) {
		printf("[+] Rebind %2d:%d to %2d and destory %2d - ", src->id, srcBranch, dst->id, mid->id);
		if (type == 0) printf("Found DoubleRef nodes on node %2d to %2d\n", mid->id, getChildNode(mid, 0)->id);
		else if (type == 1) printf("Found Redundent nodes on node %2d pointer to node %2d and node %2d\n", mid->id, getChildNode(mid, 0)->id, getChildNode(mid, 1)->id);
	}

	src->childNode[srcBranch] = dst;

	dst->rootNode.erase(std::remove_if(
		dst->rootNode.begin(),
		dst->rootNode.end(),
		[mid](const std::pair<node*, int>& p) { return p.first == mid && p.second == 0; }),
		dst->rootNode.end());

	dst->rootNode.erase(std::remove_if(
		dst->rootNode.begin(),
		dst->rootNode.end(),
		[mid](const std::pair<node*, int>& p) { return p.first == mid && p.second == 1; }),
		dst->rootNode.end());


	delete mid;
	return;
}

bool optimizationDoubleReference(node* caseNode) {

	if (getChildNode(caseNode, 0) == getChildNode(caseNode, 1)) {
		rebindLinknlist(getRootNode(caseNode), getChildNode(caseNode, 0), caseNode, getRootBranch(caseNode), 0);
		return true;
	}
	return false;
}

void optimizationRedundent(std::vector<node*> nodes) {

	while (nodes.size() >= 2) {
		for (unsigned int i = 1; i < nodes.size(); i++) {
			if (getChildNode(nodes[0], 0) == getChildNode(nodes[i], 0) &&
				getChildNode(nodes[0], 1) == getChildNode(nodes[i], 1)) {
				rebindLinknlist(getRootNode(nodes[i]), nodes[0], nodes[i], getRootBranch(nodes[i]), 1);
				nodes.erase(nodes.begin() + i); i--;
			}
		}
		nodes.erase(nodes.begin(), nodes.begin() + 1);
	}
	return;
}

void optimizationNodes(PLAdata data, node* root) {
	using namespace std;

	set<node*> nodes;
	vector<node*> sameLevelNodes;

	getAllLinkListPointer(nodes, root);

	for (unsigned int i = data.element - 1; i > 1; i--) {
		if (verbose) cout << "[+] -------- Optimization Layer " << i << " Nodes --------\n";
		for (auto& s : nodes) if (s->index == i) if (!optimizationDoubleReference(s)) sameLevelNodes.push_back(s);
		optimizationRedundent(sameLevelNodes);
		sameLevelNodes.clear();
	}

	if (verbose) cout << "\n[+] Success optimization\n";
	
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

	auto printLine = [&](node* currentNode) -> void {
		for (unsigned int i = 0; i < currentNode->childNode.size(); i++) {
			cout << PADDING << currentNode->id << " -> " << getChildNode(currentNode, i)->id << "[label=\"" << i << "\", style=";
			if (i == 0) cout << "dotted";
			else cout << "solid";
			cout << "]" << endl;
		}
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
	for (unsigned int i = 1; i < data.element-1; i++) for (auto& s : nodes) if (s->index == i) printLine(s);
	for (auto& s : nodes) if (s->index == data.element - 1) printLine(s);

	cout << "}" << endl;

	return;
}
