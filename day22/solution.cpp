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

#include "point.h"
#include "split.h"

using namespace std;

struct instruction_t {
	bool turn_on = false;
	point_t min = point_t(0, 0, 0);
	point_t max = point_t(0, 0, 0);
};

using data_t = vector<instruction_t>;
using result_t = uint64_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Part 1 */
result_t part1([[maybe_unused]] const data_t &data) {
	// return to_string(data.size());

	std::unordered_set<point_t> on_points;

	for (auto &instr : data) {
		cout << (instr.turn_on ? "on " : "off ")
			 << "x=" << instr.min.x << ".." << instr.max.x << ","
			 << "y=" << instr.min.y << ".." << instr.max.y << ","
			 << "z=" << instr.min.z << ".." << instr.max.z
			 << endl;

		if (instr.min.x < -50 || instr.max.x > 50 ||
			instr.min.y < -50 || instr.max.y > 50 ||
			instr.min.z < -50 || instr.max.z > 50) {
			cout << "  skipping out of bounds instruction" << endl;
			continue;
		}

		for (dimension_t x = instr.min.x; x <= instr.max.x; x++) {
			for (dimension_t y = instr.min.y; y <= instr.max.y; y++) {
				for (dimension_t z = instr.min.z; z <= instr.max.z; z++) {
					if (instr.turn_on) {
						on_points.emplace(x, y, z);
						// cout << "on " << x << "," << y << "," << z << endl;
					} else {
						on_points.erase(point_t(x, y, z));
						// cout << "off " << x << "," << y << "," << z << endl;
					}
				}
			}
		}
	}

	cout << on_points.size() << endl;

	return 0;
}

result_t part2([[maybe_unused]] const data_t &data) {
	// return to_string(0);
	return 0;
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	std::string line;
	while (std::getline(ifs, line)) {
		auto parts = split_str(line, " ,.=..");
		if (parts.size() == 10) {
			instruction_t instr;
			instr.turn_on = (parts[0] == "on");
			long x_min = atoi(parts[2].c_str());
			long x_max = atoi(parts[3].c_str());
			long y_min = atoi(parts[5].c_str());
			long y_max = atoi(parts[6].c_str());
			long z_min = atoi(parts[8].c_str());
			long z_max = atoi(parts[9].c_str());

			instr.min = point_t(x_min, y_min, z_min);
			instr.max = point_t(x_max, y_max, z_max);

			assert(instr.min.x <= instr.max.x);
			assert(instr.min.y <= instr.max.y);
			assert(instr.min.z <= instr.max.z);

			data.push_back(instr);
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
