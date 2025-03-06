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
#include <print>

#include "split.h"

using namespace std;

struct player_t {
	int position;
	uint64_t score;

	player_t(int position) : position(position), score(0) {
	}
};

using data_t = vector<player_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

void ten_sided_turn(data_t &players, int turn) {
	int roll = 3 * turn + (3 * turn - 1) + (3 * turn - 2);
	auto &player = (turn % 2 == 1) ? players[0] : players[1];

	int position = (((player.position-1) + roll) % 10) + 1;
	player.position = position;
	player.score += (uint64_t)position;
	// int player_n = (turn % 2 == 1) ? 1 : 2;
	// print("turn={} roll={} player={} pos={} score={}\n", 
	// 		turn, roll, player_n, player.position, player.score);
}

uint64_t max_score(const data_t &players) {
	uint64_t m_score = 0;
	for (const auto &p : players) {
		m_score = std::max(m_score, p.score);
	}

	return m_score;
};

/* Part 1 */
const result_t part1(const data_t &starting_players) {
	const uint64_t winning_score = 1000;
	data_t players = starting_players;

	int turn = 1;
	while (max_score(players) < winning_score) {
		ten_sided_turn(players, turn++);
	}
	
	turn--;

	uint64_t rolls = (uint64_t)turn * 3;
	uint64_t result = players[0].score < players[1].score ? players[0].score : players[1].score;
	result *= rolls;

	return to_string(result);
}

const result_t part2(const data_t &starting_players) {
	const uint64_t winning_score = 21;
	data_t players = starting_players;

	int turn = 1;
	while (max_score(players) < winning_score) {
		ten_sided_turn(players, turn++);
	}
	
	turn--;

	uint64_t rolls = (uint64_t)turn * 3;
	uint64_t result = players[0].score < players[1].score ? players[0].score : players[1].score;
	result *= rolls;

	return to_string(result);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto nums = split_int(line);
			assert(nums.size() == 2);
			data.push_back({nums[1]});
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
