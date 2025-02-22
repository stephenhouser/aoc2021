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
#include <algorithm>

#include "split.h"

using namespace std;

using rules_t = unordered_map<size_t, char>;
using data_t = pair<string, rules_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

template <typename Ta, typename Tb>
size_t pair_hash(const Ta a, const Tb b) {
	return ((size_t)a & 0xFF) << 8 | ((size_t)b & 0xFF);
}

pair<char, char> pair_unhash(const size_t p) {
	return {(char)((p >> 8) & 0xFF), (char)(p & 0xFF)};
}

unordered_map<char, size_t> fast_fold(const string &pattern, size_t time, const rules_t &rules) {
	unordered_map<char, size_t> counts;		// N, 1024  count of chars we have added
	unordered_map<size_t, size_t> pairs;	// NN, 32, count of pairs in consideration

	// Break initial pattern into pairs of characters. These are our inital 
	// population of "pairs". 
	// Also count the characters that are in the starting pattern as our
	// initial count of all characters that are produced.
	for (size_t i = 0; i < pattern.size() - 1; i++) {
		pairs[pair_hash(pattern[i], pattern[i+1])]++;

		counts[pattern[i]]++;
	}
	counts[pattern[pattern.size()-1]]++;

	// Iterate over time
	while (time--) {
		unordered_map<size_t, size_t> next;	// next set of pairs to process

		// for each pair in the queue, split it and add two new pairs that get
		// created by the rule, e.g. NN -> NC, CN when NN->C in the rules.

		// Also increment the number (count) of those pairs that now exist.
		// Add to the number of produced characters as well (these will be our result)
		for (const auto [pair, count] : pairs) {
			if (rules.contains(pair)) {
				auto [c1, c2] = pair_unhash(pair);
				auto product = rules.at(pair);

				// add two new pairs
				next[pair_hash(c1, product)] += count;
				next[pair_hash(product, c2)] += count;

				// update produced character count
				counts[product] += count;
			}
		}

		pairs.clear();
		swap(pairs, next);
	}

	return counts;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	const auto &[pattern, rules] = data;

	auto char_counts = fast_fold(pattern, 10, rules);
	auto [min, max] = ranges::minmax(char_counts | views::values);

	return to_string(max - min);
}

const result_t part2(const data_t &data) {
	const auto &[pattern, rules] = data;

	auto char_counts = fast_fold(pattern, 40, rules);
	auto [min, max] = ranges::minmax(char_counts | views::values);

	return to_string(max - min);
}

const data_t read_data(const string &filename) {
	string pattern;
	rules_t rules;

	std::ifstream ifs(filename);

	if (getline(ifs, pattern)) {
		string line;
		while (getline(ifs, line)) {
			auto parts = split(line);
			if (parts.size() >= 3) {
				auto pair = pair_hash(parts[0][0], parts[0][1]);
				auto product = parts[2][0];
				rules[pair] = product;
			}
		}	
	}

	return {pattern, rules};
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
