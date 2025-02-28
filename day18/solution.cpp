#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <format>
#include <print>
#include <cmath>

using namespace std;

struct node_t {
	node_t *parent = nullptr;

	node_t() {
	}

	// copy constructor
	node_t(const node_t&p) {
		this->parent = p.parent;
	}

	// assignment operator
	node_t &operator=(const node_t &other) {
		this->parent = other.parent;
		return *this;
	}

	virtual void show() const {
		print("node");
	}

	virtual ~node_t() {
	}
};

struct value_t : node_t {
	int value;

	value_t(int v) : value(v) {
	}

	void show() const {
		print("{}", this->value);
	}
};

struct pair_t : node_t {
	node_t *left = nullptr;
	node_t *right = nullptr;

	void show() const {
		print("[");
		if (this->left != nullptr) {
			this->left->show();
		}
		print(",");
		if (this->right != nullptr) {
			this->right->show();
		}
		print("]");
	}
};


using data_t = vector<node_t *>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

void in_order(node_t *node, vector<value_t *> &nodes) {
	value_t *value = dynamic_cast<value_t *>(node);
	if (value != nullptr) {
		nodes.emplace_back(value);
		return;
	}

	pair_t *pair = dynamic_cast<pair_t *>(node);
	in_order(pair->left, nodes);
	in_order(pair->right, nodes);
}

bool is_value(node_t *node) {
	return dynamic_cast<value_t *>(node) != nullptr;
}

size_t max_depth(node_t *node) {
	value_t *value = dynamic_cast<value_t *>(node);
	if (value != nullptr) {
		return 0;
	}

	pair_t *pair = dynamic_cast<pair_t *>(node);
	return max(max_depth(pair->left), max_depth(pair->right)) + 1;
}

size_t find(node_t *node, vector<value_t *> order) {
	for (size_t i = 0; i < order.size(); i++) {
		if (node == order[i]) {
			return i;			
		}
	}

	return order.size();
}

value_t *prev(node_t *node, vector<value_t *> &order) {
	size_t node_i = find(node, order);
	if (0 < node_i && node_i < order.size()) {
		return order[node_i-1];
	}

	return nullptr;
}

value_t *next(node_t *node, vector<value_t *> &order) {
	size_t node_i = find(node, order);
	if (node_i < order.size()-1) {
		return order[node_i+1];
	}

	return nullptr;
}

bool explode(node_t *node, const int depth, vector<value_t *> order) {
	if (is_value(node)) {
		return false;
	}

	pair_t *pair = dynamic_cast<pair_t *>(node);
	if (depth < 4) {
		if (explode(pair->left, depth+1, order)) {
			return true;
		}

		if (explode(pair->right, depth+1, order)) {
			return true;
		}

		return false;
	}

	value_t *left = dynamic_cast<value_t *>(pair->left);
	value_t *right = dynamic_cast<value_t *>(pair->right);

	auto p = prev(left, order);
	if (p) {
		p->value += left->value;
	}

	auto n = next(right, order);
	if (n) {
		n->value += right->value;
	}

	value_t *zero = new value_t(0);
	pair_t *parent = dynamic_cast<pair_t *>(node->parent);
	zero->parent = parent;
	if (parent->left == node) {
		parent->left = zero;
	} else {
		parent->right = zero;
	}

	delete node;
	return true;
}

bool split(node_t *node) {
	// print("split ");
	value_t *value = dynamic_cast<value_t *>(node);
	if (value != nullptr) {
		// print("value {}\n", value->value);
		if (value->value > 9) {
			double half = (double)value->value / 2.0;
			pair_t *pair = new pair_t();
			pair->left = new value_t((int)floor(half));
			pair->left->parent = pair;
			pair->right = new value_t((int)ceil(half));
			pair->right->parent = pair;
			
			pair_t *parent = dynamic_cast<pair_t *>(node->parent);
			pair->parent = parent;
			if (parent->left == node) {
				parent->left = pair;
			} else {
				parent->right = pair;
			}

			delete node;
			return true;
		}

		return false;
	}

	// print("pair\n");
	pair_t *pair = dynamic_cast<pair_t *>(node);
	return split(pair->left) || split(pair->right);
}

node_t *reduce(node_t *node) {
	vector<value_t *> order;

	do {
		order.clear();
		in_order(node, order);
		// node->show();
		// print("\n");
	} while (explode(node, 0, order) || split(node));

	return node;
}

node_t *add_nodes(node_t* l, node_t *r) {
	pair_t *root = new pair_t();
	root->left = l;
	l->parent = root;

	root->right = r;
	r->parent = root;

	reduce(root);
	return root;
}

node_t *parse_node(const string &str, size_t &pos, node_t *parent = nullptr) {
	if (isdigit(str[pos])) {
		// make a value node
		value_t *node = new value_t(stoi(str.substr(pos++, 1)));
		node->parent = parent;
		return node;	
	} else if (str[pos] == '[') {
		// make a pair node
		pair_t *node = new pair_t();
		node->parent = parent;

		node->left = parse_node(str, ++pos, node);
		// skip ,
		assert(str[pos] == ',');
		node->right = parse_node(str, ++pos, node);
		// skip ]
		assert(str[pos] == ']');
		pos++;
		return node;
	}

	print("ERROR ({}) [{}]\n", pos, str.substr(pos));
	return nullptr;
}

void test_explode() {
	size_t pos = 0;
	// node_t *t1 = parse_node("[[[[[9,8],1],2],3],4]", pos);
	// t1->show();
	// print("\n");

	// reduce(t1);
	// t1->show();
	// print("\n");

	node_t *t2 = parse_node("[7,[6,[5,[4,[3,2]]]]]", pos);
	t2->show();
	print("\n");

	reduce(t2);
	t2->show();
	print("\n");
}

size_t eval(node_t *node) {
	value_t *value = dynamic_cast<value_t *>(node);
	if (value != nullptr) {
		return (size_t)value->value;
	}

	pair_t *pair = dynamic_cast<pair_t *>(node);
	return (3 * eval(pair->left)) + (2 * eval(pair->right));
}



/* Part 1 */
const result_t part1(const data_t &data) {
	node_t *root = data[0];
	// print("  ");
	// root->show();
	// print("\n");

	for (size_t i = 1; i < data.size(); i++) {
		// print("+ ");
		node_t *node = data[i];
		// node->show();
		// print("\n");

		root = add_nodes(root, node);
		// print("= ");
		// root->show();
		// print("\n\n");
	}

	size_t result = eval(root);
	return to_string(result);
}

const result_t part2(const data_t &data) {
	size_t result = 0;

	for (size_t i = 0; i < data.size(); i++) {
		data[i]->show();
		print("\n");
		for (size_t j = i; j < data.size(); j++) {
			print("\t");
			data[j]->show();
			print("\n");

			auto root = add_nodes(data[i], data[j]);
			auto local = eval(root);

			print("==> {}\n", local);
			result = max(result, local);
		}
	}

	return to_string(result);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			size_t pos = 0;
			data.push_back(parse_node(line, pos));
		}
	}

	return data;
}

template <typename T>
void print_result(T result, chrono::duration<double, milli> duration) {
	const int time_width = 10;
	const int time_precision = 4;
	const int result_width = 15;

	cout << std::setw(result_width) << std::right << result;

	cout << " ("
		 << std::setw(time_width) << std::fixed << std::right 
		 << std::setprecision(time_precision)
		 << duration.count() << "ms)";
	cout << endl;
}

int main(int argc, char *argv[]) {
	const char *input_file = argv[1];
	if (argc < 2) {
		input_file = "test.txt";
	}

    auto start_time = chrono::high_resolution_clock::now();

	auto data = read_data(input_file);

	auto parse_time = chrono::high_resolution_clock::now();
	print_result("parse", (parse_time - start_time));

	// result_t p1_result = part1(data);
	result_t p1_result = "0";

	auto p1_time = chrono::high_resolution_clock::now();
	print_result(p1_result, (p1_time - parse_time));

	result_t p2_result = part2(data);

	auto p2_time = chrono::high_resolution_clock::now();
	print_result(p2_result, (p2_time - p1_time));

	print_result("total", (p2_time - start_time));
}
