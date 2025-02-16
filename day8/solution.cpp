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

#include "split.h"

using namespace std;

// vector of <patterns, output>
using data_t = vector<pair<vector<string>, vector<string>>>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Part 1 */
const result_t part1(const data_t &data) {
	size_t result = 0;

	vector<string> digit_pattern(10, "");

	for (const auto &[patterns, output] : data) {
		cout << "patterns=" << patterns << "  ";
		cout << "output=" << output << endl;
		
		for (const auto &value : output) {
			if (value.size() == 2 /* 1 */ || 
				value.size() == 3 /* 7 */ ||
				value.size() == 4 /* 4 */ ||
				value.size() == 7 /* 8 */) {
					result++;
				}
		}
	}

	return to_string(result);
}

const result_t part2([[maybe_unused]] const data_t &data) {
	return to_string(0);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	vector<string> patterns;
	vector<string> output;

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto delim_pos = line.find('|');
			string line_patterns = line.substr(0, delim_pos);
			string line_output = line.substr(delim_pos+1);
			data.push_back({split_str(line_patterns), split_str(line_output)});
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
