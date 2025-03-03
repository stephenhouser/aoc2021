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
#include <print>

#include "point.h"

using namespace std;

uint64_t distance(const point_t &p1, const point_t &p2) {
	uint64_t dx = (uint64_t)abs(p1.x - p2.x);
	uint64_t dy = (uint64_t)abs(p1.y - p2.y);
	uint64_t dz = (uint64_t)abs(p1.z - p2.z);
	return dx + dy + dz;
}

struct scanner_t {
	uint64_t id;
	vector<point_t> points;
	unordered_map<uint64_t, uint64_t> distances = {};

	scanner_t(const uint64_t n, const vector<point_t> &points) : id(n), points(points) {
		set_distances();
	}

	private:
	void set_distances() {
		for (size_t i = 0; i < points.size(); i++) {
			for (size_t j = i+1; j < points.size(); j++) {
				this->distances[distance(points[i], points[j])]++;
			}
		}
	}
};

using data_t = vector<scanner_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

// bool match_scanner(const vec)
uint64_t common_distances(const scanner_t &s1, const scanner_t &s2) {
	uint64_t common = 0;
	for (const auto [distance, count] : s1.distances) {
		if (s2.distances.contains(distance)) {
			common += min(count, s2.distances.at(distance));
		}
	}

	return common;
}

// 3d transform matrix
using matrix_t = array<array<uint64_t, 4>, 4>;

/* 
 translation
	1  0  0  0
	0  1  0  0
	0  0  1  0
	dx dy dz 1

rotate z
	cos -sin 0 0
	sin  cos 0 0
	0      0 1 0
	0      0 0 1

rotate y
	cos  0 sin 0
	0    1   0 0
	-sin 0 cos 0
	0    0   0 1

rotate x
	1   0    0 0
	0 cos -sin 0
	0 sin  cos 0
	0   0    0 1

*/
vector<vector<point_t>> rotations = {
	{{ 1,  0,  0}, { 0,  1,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 0,  1,  0}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  1,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 0,  1,  0}, { 1,  0,  0}},

	{{ 0, -1,  0}, { 1,  0,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 1,  0,  0}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 1,  0,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 1,  0,  0}, { 0, -1,  0}},

	{{ 0,  1,  0}, {-1,  0,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, {-1,  0,  0}, { 0, -1,  0}},
	{{ 0, -1,  0}, {-1,  0,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, {-1,  0,  0}, { 0,  1,  0}},

	{{ 1,  0,  0}, { 0,  0, -1}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 0,  0, -1}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  0, -1}, { 0, -1,  0}},
	{{ 0, -1,  0}, { 0,  0, -1}, { 1,  0,  0}},

	{{ 1,  0,  0}, { 0, -1,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 0, -1,  0}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0, -1,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 0, -1,  0}, { 1,  0,  0}},

	{{ 1,  0,  0}, { 0,  0,  1}, { 0, -1,  0}},
	{{ 0, -1,  0}, { 0,  0,  1}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  0,  1}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 0,  0,  1}, { 1,  0,  0}},
};

point_t rotate(const point_t &p, const size_t n) {
	point_t result;
    vector<point_t> rot = rotations[n];
    result.x = p.x * rot[0].x + p.y * rot[0].y + p.z * rot[0].z;
    result.y = p.x * rot[1].x + p.y * rot[1].y + p.z * rot[1].z;
    result.z = p.x * rot[2].x + p.y * rot[2].y + p.z * rot[2].z;
    return result;
}

point_t transform(const matrix_t &mx, const point_t &p) {
	// w coord is 1
    point_t result;
	uint64_t w = p.x * mx[0][3] + p.y * mx[1][3] + p.z + mx[2][3] + 1 * mx[3][3];
	result.x = (p.x * mx[0][0] + p.y * mx[1][0] + p.z + mx[2][0] + 1 * mx[3][0]) / w;
	result.y = (p.x * mx[0][1] + p.y * mx[1][1] + p.z + mx[2][1] + 1 * mx[3][1]) / w;
	result.z = (p.x * mx[0][2] + p.y * mx[1][2] + p.z + mx[2][2] + 1 * mx[3][2]) / w;

    return result;
}

matrix_t compose(const matrix_t &first, const matrix_t&second) {
    matrix_t result;
    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            result[i][j] = first[i][0] * second[0][j]
                           + first[i][1] * second[1][j]
                           + first[i][2] * second[2][j]
                           + first[i][3] * second[3][j];
		}
	}

    return result;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	auto scanners = data;
	for (uint64_t i = 0; i < scanners.size(); i++) {
		for (uint64_t j = i+1; j < scanners.size(); j++) {
			uint64_t common = common_distances(scanners[i], scanners[j]);
			if (common >= (12+11+10+9+8+7+6+5+4+3+2+1)) {
				print("scanner {} and {} have {} distances in common\n", 
					scanners[i].id, scanners[j].id, common);
			}
		}
	}

	return to_string(0);
}

const result_t part2([[maybe_unused]] const data_t &data) {
	return to_string(0);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	uint64_t scanner_n = 0;
	vector<point_t> scanner;
	while (getline(ifs, line)) {
		if (line.starts_with("---")) {
			scanner_t scanner(scanner_n++, read_points(ifs));
			data.push_back(scanner);
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
