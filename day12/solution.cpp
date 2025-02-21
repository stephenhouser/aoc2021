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
#include <unordered_map>
#include <functional>
#include <cctype>
#include <unordered_set>

#include "split.h"

using namespace std;

// name -> neighbors
using data_t = unordered_map<string, vector<string>>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

bool is_end_cave(const string &name) {
	return name == "start" || name == "end";
}

bool is_big_cave(const string &name) {
	return 'A' <= name[0] && name[0] <= 'Z';
}

bool is_small_cave(const string &name) {
	return !is_big_cave(name);
}

bool in_path(const string &cave, const vector<string> &path) {
	for (const auto &p : path) {
		if (p == cave) {
			return true;
		}
	}

	return false;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	vector<vector<string>> paths;

	// breadth first search, collecting paths when we reach "end"
	std::function<void(const data_t &, const string &, vector<string>)> bfs;
	bfs = [&paths, &bfs](const data_t &cavemap, const string &current, vector<string> path) {
		path.emplace_back(current);
		if (current == "end") {
			paths.emplace_back(path);
			return;
		}

		// iterate over all neighbors of the current node (breadth first)
		for (const auto &neighbor : cavemap.at(current)) {
			if (!is_small_cave(neighbor) || !in_path(neighbor, path)) {
				bfs(cavemap, neighbor, path);
			}
		}
	};
	
	bfs(data, "start", {});


	return to_string(paths.size());
}

string flatten(const vector<string> &vec) {
	string result;
	for (const auto &s : vec) {
		if (result.size()) {
			result.append(",");
		}
		result.append(s);
	}

	return result;
}

const result_t part2(const data_t &data) {
	// maintain as set of strings for easy de-duplication
	unordered_set<string> paths;

	// breadth first search, collecting paths when we reach "end"
	std::function<void(const data_t, const string &, const string &, vector<string>)> bfs;
	bfs = [&paths, &bfs](const data_t cavemap, 
							const string &current, const string &promoted, 
							vector<string> path) {
		path.emplace_back(current);
		if (current == "end") {
			paths.insert(flatten(path));
			return;
		}

		// iterate over all neighbors of the current node (breadth first)
		for (const auto &neighbor : cavemap.at(current)) {
			if (is_big_cave(neighbor) || !in_path(neighbor, path)) {
				bfs(cavemap, neighbor, promoted, path);
			} else if (neighbor == promoted) {
				bfs(cavemap, neighbor, "", path);
			}
		}
	};
	
	for (const auto &[cave, neighbors] : data) {
		if (is_small_cave(cave) && cave != "start" && cave != "end") {
			bfs(data, "start", cave, {});
		}
	}

	// for (const auto &path : paths) {
	// 	cout << path << endl;
	// }
	return to_string(paths.size());
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto nodes = split_str(line, "-");
			assert(nodes.size() == 2);

			auto lhs = nodes[0];
			auto rhs = nodes[1];

			data[lhs].emplace_back(rhs);
			data[rhs].emplace_back(lhs);
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

	result_t p1_result = part1(data);

	auto p1_time = chrono::high_resolution_clock::now();
	print_result(p1_result, (p1_time - parse_time));

	result_t p2_result = part2(data);

	auto p2_time = chrono::high_resolution_clock::now();
	print_result(p2_result, (p2_time - p1_time));

	print_result("total", (p2_time - start_time));
}
