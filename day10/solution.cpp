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

using namespace std;

using data_t = vector<string>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

size_t reduce_pattern(string &src, const string &pattern) {
	size_t removed = 0;
	for (auto pos = src.find(pattern); pos != src.npos; pos = src.find(pattern)) {
		removed += 2;
		src.erase(pos, 2);
	}

	return removed;
}

string reduce_string(const string &src) {
	string work{src};
	size_t removed = 0;
	do {
		removed = reduce_pattern(work, "()");
		removed += reduce_pattern(work, "{}");
		removed += reduce_pattern(work, "[]");
		removed += reduce_pattern(work, "<>");
	} while (removed > 0);

	return work;
}

size_t score_corrupt_string(const string &str) {
	auto score_char = [](const char ch) {
		const unordered_map<char, size_t> char_scores = {
			{')', 3}, {']', 57}, {'}', 1197}, {'>', 25137} 
		};
	
		return char_scores.contains(ch) ? char_scores.at(ch) : 0;
	};

	auto pos = str.find_first_of(")}]>");
	if (pos != str.npos)  {
		return score_char(str[pos]);
	}

	return (size_t)0;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	// is this reduced string a corrupt string
	auto is_corrupt = [](const string &str) {
		return str.find_first_of(")}]>") != str.npos;
	};

	auto scores = data |
		views::transform(reduce_string) |
		views::filter(is_corrupt) |
		views::transform(score_corrupt_string);

	size_t result = reduce(scores.begin(), scores.end());
	return to_string(result);
}


size_t score_closing_string(const string &str) {
	auto score_char = [](const char ch) {
		const unordered_map<char, size_t> char_scores = {
			{')', 1}, {']', 2}, {'}', 3}, {'>', 4} 
		};
	
		return char_scores.contains(ch) ? char_scores.at(ch) : 0;
	};

	size_t score = 0;
	for (size_t i = 0; i < str.size(); i++) {
		score = (score * 5) + score_char(str[i]);
	}

	return score;
}

const result_t part2(const data_t &data) {
	// is this reduced string an incomplete string
	auto is_incomplete = [](const string &str) {
		return str.find_first_of(")}]>") == str.npos;
	};

	// 
	auto closing_chars = [](const string &str) {
		string closer;
		for (auto ch : str) {
			switch (ch) {
				case '(': closer.push_back(')'); break;
				case '{': closer.push_back('}'); break;
				case '[': closer.push_back(']'); break;
				case '<': closer.push_back('>'); break;
				default: break;
			}
		}
		return closer;
	};

	// score the strings needed to close each incomplete string
	auto scores = data | 
		views::transform(reduce_string) |
		views::filter(is_incomplete) | 
		views::transform(closing_chars) |
		views::transform(score_closing_string) |
		ranges::to<vector<size_t>>();

	// answer is the middle score from sorted list of scores
	ranges::sort(scores);
	size_t result = scores[scores.size()/2];

	return to_string(result);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(line);
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
