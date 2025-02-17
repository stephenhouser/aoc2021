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

#include "point.h"
#include "charmap.h"

using namespace std;

using data_t = charmap_t;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Part 1 */
const result_t part1(const data_t &map) {
	auto is_low = [&map](const point_t &point, char ch) {
		// is a low point if all neighbors have larger value
		auto neighbors = map.neighbors_of(point);
		return all_of(neighbors.begin(), neighbors.end(), [ch](const auto &n) {
				const auto &[neighbor, n_ch] = n;
				return ch < n_ch;
			});
	};

	auto lows = map.all_points() |
		views::filter([&map, is_low](const auto &p) {	// keep low points
			const auto &[point, p_ch] = p;
			return is_low(point, p_ch);
		}) |
		views::transform([](const auto &p) {			// transform to risk level
			return (size_t)(p.second + 1) - '0';
		}) |
		ranges::to<vector<size_t>>();					// back to vector

	size_t result = reduce(lows.begin(), lows.end());
	return to_string(result);
}

const result_t part2(const data_t &map) {
	return to_string(map.size_x);
}

const data_t read_data(const string &filename) {
	return charmap_t::from_file(filename);
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
