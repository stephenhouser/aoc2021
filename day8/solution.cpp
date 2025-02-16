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
#include <unordered_set>

#include "split.h"

using namespace std;

// vector of <patterns, output>
using data_t = vector<pair<vector<string>, vector<string>>>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


unordered_set<char> set_for(const string &str) {
	unordered_set<char> chars(str.begin(), str.end());
	return chars;
}

using pattern_t = unordered_set<char>;

string difference(const string &p1, const string &p2) {
	string result;
	for (const char p : p1) {
		if (!p2.contains(p)) {
			result.push_back(p);
		}
	}

	return result;
}


/* [0] is the char pattern for 0, [1] is the char pattern for 1*/
const vector<string> decode(const vector<string> &patterns) {
	vector<string> digit_pattern(10, "");
	vector<string> six_seg;
	vector<string> five_seg;

	// find 1, 7, 4, and 8, the "easy ones" and queue the others
	for (const auto &pattern : patterns) {
		switch (pattern.size()) {
			case 2:	// 1
				digit_pattern[1] = pattern;
				break;
			case 3:	// 7
				digit_pattern[7] = pattern;
				break;
			case 4:	// 4
				digit_pattern[4] = pattern;
				break;
			case 5:	// 4
				five_seg.push_back(pattern);
				break;
			case 6:
				six_seg.push_back(pattern);
				break;
			case 7:	// 8
				digit_pattern[8] = pattern;
				break;
		}
	}
	// 2, 3, 5
	assert(five_seg.size() == 3);
	for (const auto &pattern : five_seg) {
		if (difference(digit_pattern[1], pattern).size() == 0) {
			digit_pattern[3] = pattern;
		} else if (difference(pattern, digit_pattern[4]).size() == 2) {
			digit_pattern[5] = pattern;
		} else {
			digit_pattern[2] = pattern;
		}
	}

	// 0, 6, 9
	assert(six_seg.size() == 3);
	for (const auto &pattern : six_seg) {
		if (difference(digit_pattern[1], pattern).size() == 1) {
			digit_pattern[6] = pattern;
		} else if (difference(pattern, digit_pattern[4]).size() == 2) {
			digit_pattern[9] = pattern;
		} else {
			digit_pattern[0] = pattern;
		}
	}

	return digit_pattern;
}

size_t decode_digit(const string &code, const vector<string> &digit_codes) {
	for (size_t digit = 0; digit < digit_codes.size(); digit++) {
		if (code == digit_codes[digit]) {
			return digit;
		}
	}

	return 0;
}

size_t decode(const vector<string> &codes, const vector<string> &digit_codes) {
	size_t decoded = 0;
	for (const auto &code : codes) {
		auto digit = decode_digit(code, digit_codes);
		cout << "\t" << code << " = " << digit << endl;
		decoded = (decoded * 10) + digit;
	}

	return decoded;
}

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

const result_t part2(const data_t &data) {
	size_t result = 0;

	for (const auto &[patterns, output] : data) {
		auto digit_patterns = decode(patterns);
		size_t intermediate = decode(output, digit_patterns);

		cout << output << " => " << intermediate << endl;

		result += intermediate;
	}

	return to_string(result);
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
