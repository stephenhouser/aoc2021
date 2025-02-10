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

using data_t = vector<string>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Return a pair indicating how many zeros and ones there are
 * in the n-th bit across a vector of strings (of 0's and 1's).
 */
pair<size_t, size_t> count_bits(const data_t &data, size_t bit) {
	pair<size_t, size_t> bit_count;

	for (const auto &s : data) {
		if (s[bit] == '0') {
			bit_count.first++;
		} else {
			bit_count.second++;
		}
	}

	return bit_count;
}

/* Return a vector of pairs indicating how many zeros and ones there are
 * in each bit position (column) across the vector of strings (of 1's and 0's)
 */
vector<pair<size_t, size_t>> count_all_bits(const data_t &data) {
	vector<pair<size_t, size_t>> bit_counts;

	for (size_t i = 0; i < data[0].size(); i++) {
		bit_counts.push_back(count_bits(data, i));
	}

	return bit_counts;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	auto bits = count_all_bits(data);

	// create an unsigned int for the most common bits
	size_t mcb = accumulate(bits.begin(), bits.end(), (size_t)0,
		[](size_t result, pair<size_t, size_t> bit) {
			return (result << 1) | (bit.second > bit.first);
	});

	// compute least common bits as inverse of most common
	// leave upper bits in long word (masked) as 0's
	size_t mask = (1 << data[0].size()) - 1ul;
	size_t lcb = ~mcb & mask;

	// result is most common * least common
	return to_string(mcb * lcb);
}

const result_t part2([[maybe_unused]] const data_t &data) {
	data_t o2_candidates{data};
	data_t co2_candidates{data};

	// readinstructions carefully.
	// We need to eliminate candidates based only on candidates
	// that remain after each round of elimination.

	for (size_t bit = 0; bit < data[0].size(); bit++) {
		// erase non-conforming candidates from each pool of candidates
		// if there are more than 1 candidate remaining		
		if (o2_candidates.size() > 1) {
			auto o2_bits = count_bits(o2_candidates, bit);
			auto o2_check = (o2_bits.second >= o2_bits.first) ? '1' : '0';
			erase_if(o2_candidates, [bit, o2_check](const string &s) {
				return s[bit] != o2_check ;
			});
		}

		if (co2_candidates.size() > 1) {
			auto co2_bits = count_bits(co2_candidates, bit);
			auto co2_check = (co2_bits.second < co2_bits.first) ? '1' : '0';
			erase_if(co2_candidates, [bit, co2_check](const string &s) {
				return s[bit] != co2_check;
			});
		}
	}

	assert(o2_candidates.size() == 1);
	assert(co2_candidates.size() == 1);

	// convert single remaining candidates to integers
	size_t o2_rating = stoul(o2_candidates[0], nullptr, 2);
	size_t co2_rating = stoul(co2_candidates[0], nullptr, 2);
	return to_string(o2_rating * co2_rating);
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
