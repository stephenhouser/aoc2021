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

using namespace std;

/* verbosity level; 0 = nothing extra, 1 = more... Set by command line. */
int verbose = 0;

using data_t = vector<string>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Part 1 */
const result_t part1(const data_t &data) {
	auto count_ones = [](vector<size_t> counts, const string &s) {
		for (size_t i = 0; i < s.size(); i++) {
			counts[i] += (s[i] == '1') ? 1 : 0;
		}
		return counts;
	};

	// accumulate to count the 1's in each bit location
	// then transform to 1 if more than half (threshold) are 1's, 0 otherwise
	// resulting vector has 1 or 0 for whichever is most common bit across data
	size_t threshold = data.size() / 2;
	vector<size_t> bit_counts(data[0].size());
	auto bits = accumulate(data.begin(), data.end(), bit_counts, count_ones) | 
		views::transform([threshold](size_t one_count) {
			return (one_count > threshold) ? 1 : 0;
		});

	// create an unsigned int for the most common bits
	size_t mcb = accumulate(bits.begin(), bits.end(), (size_t)0,
		[](size_t result, size_t bit) {
			return (result << 1) | (bit & 0x01);
		});

	// create an unsigned int for the least common bits
	size_t lcb = accumulate(bits.begin(), bits.end(), (size_t)0,
		[](size_t result, size_t bit) {
			return (result << 1) | (~bit & 0x01);
		});

	// result is most common * least common
	return to_string(mcb * lcb);
}

const result_t part2([[maybe_unused]] const data_t &data) {
	size_t result = 0;

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
