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
#include "cube.h"
#include "split.h"

using namespace std;

struct instruction_t {
	bool turn_on = false;
	cube_t cube = cube_t();
};

using data_t = vector<instruction_t>;
using result_t = uint64_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

result_t volume(const vector<cube_t> &cubes) {
	// playing with c++ algorithms

	// c++ 23 ranges
	result_t total = std::ranges::fold_left(
		cubes | std::views::transform([](const cube_t &cube) {
			return (result_t)cube.volume();
		}),
		0, std::plus<result_t>());

	// c++ 20 algorithms
	// vector<result_t> volumes;

	// std::transform(cubes.begin(), cubes.end(), std::back_inserter(volumes),
	// 	[](const cube_t &cube) {
	// 		return (result_t)cube.volume();
	// 	});

	// result_t total = std::reduce(volumes.begin(), volumes.end(), (result_t)0, std::plus<result_t>());

	// // transform to volumes and reduce to total (sum)
	// result_t total = std::transform_reduce(cubes.begin(), cubes.end(), (result_t)0,
	// 	[](const result_t acc, const result_t vol) {
	// 		return acc + vol;
	// 	},
	// 	[](const cube_t &cube) {
	// 		return (result_t)cube.volume();
	// 	});

	return total;
}

std::vector<cube_t> add_cube(std::vector<cube_t> &cubes, const cube_t &new_cube, bool turn_on) {
	std::vector<cube_t> updated_cubes;

	for (auto &existing_cube : cubes) {
		auto sub_cubes = existing_cube.subtract(new_cube);
		updated_cubes.insert(updated_cubes.end(), sub_cubes.begin(), sub_cubes.end());
	}

	if (turn_on) {
		updated_cubes.push_back(new_cube);
	}

	return updated_cubes;
}

/* Part 1 */
result_t part1([[maybe_unused]] const data_t &data) {
	std::vector<cube_t> on_cubes;

	for (auto &instr : data) {
		// cout << (instr.turn_on ? "on " : "off ") << " " << instr.cube << endl;

		// skip instructions that are out of bounds
		if (instr.cube.p1.x < -50 || instr.cube.p2.x > 50 ||
			instr.cube.p1.y < -50 || instr.cube.p2.y > 50 ||
			instr.cube.p1.z < -50 || instr.cube.p2.z > 50) {
			continue;
		}

		auto updated_cubes = add_cube(on_cubes, instr.cube, instr.turn_on);
		on_cubes = std::move(updated_cubes);
	}

	return volume(on_cubes);
}

result_t part2([[maybe_unused]] const data_t &data) {
	std::vector<cube_t> on_cubes;

	for (auto &instr : data) {
		// cout << (instr.turn_on ? "on " : "off ") << " " << instr.cube << endl;

		auto updated_cubes = add_cube(on_cubes, instr.cube, instr.turn_on);
		on_cubes = std::move(updated_cubes);
	}

	return volume(on_cubes);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	std::string line;
	while (std::getline(ifs, line)) {
		auto parts = split_str(line, " ,.=..");
		if (parts.size() == 10) {
			long x_min = atoi(parts[2].c_str());
			long x_max = atoi(parts[3].c_str());
			long y_min = atoi(parts[5].c_str());
			long y_max = atoi(parts[6].c_str());
			long z_min = atoi(parts[8].c_str());
			long z_max = atoi(parts[9].c_str());

			assert(x_min <= x_max);
			assert(y_min <= y_max);
			assert(z_min <= z_max);

			instruction_t instr;
			instr.turn_on = (parts[0] == "on");
			instr.cube = cube_t(x_min, y_min, z_min, x_max, y_max, z_max);
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
