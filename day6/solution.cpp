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

#include "split.h"

using namespace std;

using data_t = vector<size_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

void show(const unordered_map<size_t, size_t> &fish) {
	for (const auto [age, count] : fish) {
		cout << "age=" << age << " count=" << count << endl;
	}
	cout << endl;
}

size_t spawn_fish(const data_t &starting_fish, size_t days, size_t max_age = 8) {
	vector<size_t> fish(max_age+1, 0);

	// count fish at each age in starting_fish
	for (auto f : starting_fish) {
		fish[f]++;
	}

	for (size_t day = 0; day < days; day++) {
		vector<size_t> next_fish(max_age+1, 0);

		for (size_t age = 0; age <= max_age; age++) {
			size_t count = fish[age];
			if (age == 0)  {
				next_fish[6] += count;
				next_fish[8] += count;
			} else {
				next_fish[age-1] += count;
			}
		}

		swap(fish, next_fish);
	}

	size_t result = reduce(fish.begin(), fish.end());
	return result;
}

/* Part 1 */
const result_t part1(const data_t &starting_fish) {
	auto fish_count = spawn_fish(starting_fish, 80);
	return to_string(fish_count);
}

const result_t part2([[maybe_unused]] const data_t &starting_fish) {
	auto fish_count = spawn_fish(starting_fish, 256);
	return to_string(fish_count);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto fish = split_size_t(line);
			data.insert(data.end(), fish.begin(), fish.end());
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
