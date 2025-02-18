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
#include <queue>
#include <algorithm>

#include "point.h"
#include "charmap.h"

using namespace std;

using data_t = charmap_t;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Return range of low points on the map */
auto low_points(const data_t &map) {
	// test if point and char is a low point on the map.
	// true if all neighbors have larger value, false otherwise
	auto is_low = [&map](const point_t &point, char ch) {
		auto neighbors = map.neighbors_of(point);
		return all_of(neighbors.begin(), neighbors.end(), [ch](const auto &n) {
				const auto &[neighbor, n_ch] = n;
				return ch < n_ch;
			});
	};

	return map.all_points() |
		views::filter([&map, is_low](const auto &p) {	// keep low points
			const auto &[point, p_ch] = p;
			return is_low(point, p_ch);
		}) |
		views::transform([](const auto &p) {			// return only points
			return p.first;
		});
		//  |
		// ranges::to<vector<point_t>>();				// back to vector
}

/* Part 1 */
const result_t part1(const data_t &map) {
	auto risk_level = [&map](const auto &point) {
		return (size_t)(map.get(point) + 1) - '0';
	};

	// transform low points to vector of risk levels
	auto lows = low_points(map) |
		views::transform([&risk_level](const auto &point) { return risk_level(point); }) |
		ranges::to<vector<size_t>>();

	size_t result = reduce(lows.begin(), lows.end());
	return to_string(result);
}


const result_t part2(const data_t &map) {
	// /* Returns the size of the basin with low point. 
	// * basin is defined by all points connected to low bordered by '9's */
	auto basin_size = [&map](const point_t &low) {
		// flood fill
		unordered_set<point_t> confirmed;
		queue<point_t> tentative;
		tentative.push(low);

		while (!tentative.empty()) {
			auto point = tentative.front();
			tentative.pop();
			if (!confirmed.contains(point)) {
				confirmed.insert(point);

				for (const auto &[neighbor, ch] : map.neighbors_of(point)) {
					if (ch != '9') {
						tentative.push(neighbor);
					}
				}
			}
		}

		// return size of filled area (the basin)
		return confirmed.size();
	};

	// transform low points to vector of risk levels
	auto basins = low_points(map) |
		views::transform([&basin_size](const auto &point) { return basin_size(point); }) |
		ranges::to<vector<size_t>>();

	// really annoying that you cannot sort as part of a pipeline.
	ranges::sort(basins, std::greater<size_t>());
	auto top3 = basins | views::take(3) | ranges::to<vector<size_t>>();
	size_t result = reduce(top3.begin(), top3.end(), 1u, std::multiplies<size_t>());

	return to_string(result);
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
