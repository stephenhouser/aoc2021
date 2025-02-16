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
#include <cmath>

#include "split.h"

using namespace std;

using data_t = vector<size_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Part 1 */
const result_t part1(const data_t &starting_crabs) {
	// put crab subs into vector in order
	vector<size_t> crabs = starting_crabs;
	sort(crabs.begin(), crabs.end());

	// compute the median -- it will be the closest to all points
	size_t median = crabs[crabs.size()/2];

	// calculate distance of each crab to median
	auto distance = crabs |
			views::transform([median](const size_t crab) {
				return (size_t)abs((long)crab - (long)median);
			});

	// reduce (sum) to get fuel cost
	size_t fuel_cost = reduce(distance.begin(), distance.end());
	return to_string(fuel_cost);
}

/* Return cost for all crab subs to get to pos using
 * more expensive (n * (n + 1) / 2) (Gaussian sum).
 */
size_t fuel_expense(size_t pos, const data_t &crabs) {
	auto distance = crabs |
			views::transform([pos](const size_t crab) {
				size_t distance = (size_t)abs((long)crab - (long)pos);
				// (n * (n + 1)) / 2
				return (distance * (distance + 1)) / 2;
			});

	return reduce(distance.begin(), distance.end());
}

const result_t part2(const data_t &crabs) {
	// compute the average -- it will be the closest to all points
	size_t total = reduce(crabs.begin(), crabs.end());
	size_t average = total / crabs.size();

	// take the min of the average rounded down and rounded up.
	size_t fuel_cost = min(fuel_expense(average, crabs), 
						   fuel_expense(average+1, crabs));

	return to_string(fuel_cost);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto crabs = split_size_t(line);
			data.insert(data.end(), crabs.begin(), crabs.end());
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
