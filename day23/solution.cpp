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
#include <map>			// map
#include <unordered_set> // unordered_set

#include "charmap.h"

using namespace std;

struct state_t {
	std::string state;
	size_t cost;

	bool operator==(const state_t &other) const {
		return state == other.state;
	}

	bool operator!=(const state_t &other) const {
		return state != other.state;
	}

	bool operator<(const state_t &other) const {
		return cost < other.cost;
	}
};

/* hash function so can be put in unordered_map or set */
template <>
struct std::hash<state_t> {
	size_t operator()(const state_t &sc) const {
		return std::hash<std::string>()(sc.state);
	}
};

// using state_t = string;
using result_t = string;

const state_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

void show_state(const state_t &state) {
	string board = "#############\n#...........#\n###.#.#.#.###\n  #.#.#.#.#\n  #########";

	board[15] = state.state[0];
	board[16] = state.state[1];
	board[17] = state.state[2];
	board[18] = state.state[3];
	board[19] = state.state[4];
	board[20] = state.state[5];
	board[21] = state.state[6];
	board[22] = state.state[7];
	board[23] = state.state[8];
	board[24] = state.state[9];
	board[25] = state.state[10];
	board[31] = state.state[11];
	board[33] = state.state[12];
	board[35] = state.state[13];
	board[37] = state.state[14];
	board[45] = state.state[15];
	board[47] = state.state[16];
	board[49] = state.state[17];
	board[51] = state.state[18];

	cout << board << "  \t" << state.cost << endl;
}

// Valid moves from each position
// 0 -> 1
// 1 -> 0, 3	-> 11
// 3 -> 1, 5	-> 11, 12
// 5 -> 3, 7	-> 12, 13
// 7 -> 5, 9	-> 13, 14
// 9 -> 7, 10	-> 14
// 10 -> 9
// 11 -> 1, 3, 15
// 12 -> 3, 5, 16
// 13 -> 5, 7, 17
// 14 -> 7, 9, 18
// 15 -> 11
// 16 -> 12
// 17 -> 13
// 18 -> 14
struct move_t {
	size_t position;	// position index
	size_t cost;		// cost to move here
	char restriction;	// only this amphipod can go here, '.' means any
};

const std::map<size_t, std::vector<move_t>> all_moves = {
	{0, {{1, 1, '.'}}},
	{1, {{0, 1, '.'}, {3, 2, '.'}, {11, 2, 'A'}}},
	{3, {{1, 2, '.'}, {5, 2, '.'}, {11, 2, 'A'}, {12, 2, 'B'}}},
	{5, {{3, 2, '.'}, {7, 2, '.'}, {12, 2, 'B'}, {13, 2, 'C'}}},
	{7, {{5, 2, '.'}, {9, 2, '.'}, {13, 2, 'C'}, {14, 2, 'D'}}},
	{9, {{7, 2, '.'}, {10, 1, '.'}, {14, 2, 'D'}}},
	{10, {{9, 1, '.'}}},
	{11, {{1, 2, '.'}, {3, 2, '.'}, {15, 1, 'A'}}},
	{12, {{3, 2, '.'}, {5, 2, '.'}, {16, 1, 'B'}}},
	{13, {{5, 2, '.'}, {7, 2, '.'}, {17, 1, 'C'}}},
	{14, {{7, 2, '.'}, {9, 2, '.'}, {18, 1, 'D'}}},
	{15, {{11, 1, '.'}}},
	{16, {{12, 1, '.'}}},
	{17, {{13, 1, '.'}}},
	{18, {{14, 1, '.'}}},
};

const std::map<char, size_t> amphipod_cost = {
	{'A', 1},
	{'B', 10},
	{'C', 100},
	{'D', 1000},
};

const std::map<char, std::vector<size_t>> amphipod_home = {
	{'A', {11, 15}},
	{'B', {12, 16}},
	{'C', {13, 17}},
	{'D', {14, 18}},
};

bool room_contains_only(const state_t &state, char amphipod) {
	// does the amphipod's home room contain only '.' or the amphipod?
	if (amphipod_home.contains(amphipod)) {
		for (auto pos : amphipod_home.at(amphipod)) {
			if (state.state[pos] != '.' && state.state[pos] != amphipod) {
				return false;
			}
		}

		return true;
	}

	return false;
}

vector<size_t> find_amphipods(const state_t &state) {
	vector<size_t> positions;

	for (auto i : views::iota(0u, 18u)) {
		if (state.state[i] != '.') {
			positions.push_back(i);
		}
	}

	return positions;
}

// returns valid moves for amphipod at pos, not going back to from while in state
std::vector<move_t> valid_moves(const state_t &state, size_t pos, size_t from) {
	std::vector<move_t> moves;
	char amphipod = state.state[pos];

	for (auto &move : all_moves.at(pos)) {
		// can only move to unrestricted rooms or our homes
		// can't move that has other types in it
		if (move.restriction == '.' || 
			(move.restriction == amphipod && room_contains_only(state, amphipod))) {

			// can only move to '.' and places we are not coming from
			if (state.state[move.position] == '.' && move.position != from) {
				moves.push_back(move);
			}
		}
	}

	return moves;
}

// returns possible next states from state for amphipod at pos
void next_states(const state_t &state, size_t pos, size_t from, unordered_set<state_t> &states) {
	char amphipod = state.state[pos];

	for (const auto &move : valid_moves(state, pos, from)) {
		state_t new_state = state;
		swap(new_state.state[pos], new_state.state[move.position]);
		new_state.cost += amphipod_cost.at(amphipod) * move.cost;

		if (!states.contains(new_state) || states.find(new_state)->cost > new_state.cost) {
			// remove existing higher cost state
			auto existing = states.find(new_state);
			if (existing != states.end()) {
				states.erase(existing);
			}

			states.insert(new_state);
			// recursive moves, don't backtrack to where we came from
			next_states(new_state, move.position, pos, states);
		}
	}
}

// returns possible next states from state for all amphipods
unordered_set<state_t> next_states(const state_t &state) {
	unordered_set<state_t> states;

	// next_states(state, 11, 11, states);
	for (auto pos : find_amphipods(state)) {
		next_states(state, pos, pos, states);
	}

	if (states.contains(state)) {
		cout << "Removing original state." << endl;
		states.erase(state);
	}

	return states;
}

/* Part 1 */
const result_t part1([[maybe_unused]] const state_t &initial_state) {

	cout << "Initial state:" << endl;
	show_state(initial_state);

	auto states = next_states(initial_state);
	cout << "Found " << states.size() << " next states." << endl;
	for (const auto &state : states) {
		show_state(state);
		cout << endl;
	}

	return to_string(initial_state.state.size());
}

const result_t part2([[maybe_unused]] const state_t &initial_state) {
	return to_string(0);
}

const state_t read_data(const string &filename) {
	charmap_t map = charmap_t::from_file(filename);

	string data = "...........";
	data.append(1, map.get(3, 2));
	data.append(1, map.get(5, 2));
	data.append(1, map.get(7, 2));
	data.append(1, map.get(9, 2));

	data.append(1, map.get(3, 3));
	data.append(1, map.get(5, 3));
	data.append(1, map.get(7, 3));
	data.append(1, map.get(9, 3));

	state_t initial_state;
	initial_state.state = data;
	initial_state.cost = 0;

	return initial_state;
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
