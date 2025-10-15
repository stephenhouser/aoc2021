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
#include <functional>
#include <climits>

#include "bitstream.h"

using namespace std;

using data_t = vector<string>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

struct packet_t {
	size_t version = 0x00;
	size_t id = 0x00;
	size_t value = 0x00;
	vector<packet_t> sub = {};
};

packet_t decode(bitstream_t &bits);
size_t decode_literal(bitstream_t &bits);
vector<packet_t> decode_subpackets(bitstream_t &bits);

size_t eval(const packet_t &packet);


void show(const packet_t &packet, const int level = 0) {
	for (int i = 0; i < level; i++) {
		cout << "\t";
	}

	cout << "version=" << packet.version << " id=" << packet.id;
	switch (packet.id) {
		case 0: cout << " sum="; break;
		case 1: cout << " product="; break;
		case 2: cout << " minimum="; break;
		case 3: cout << " maximum="; break;
		case 4: cout << " literal="; break;
		case 5: cout << " greater="; break;
		case 6: cout << " less="; break;
		case 7: cout << " equal="; break;
	}
	cout << eval(packet) << endl;

	if (packet.id != 4) {
		for (const auto &p : packet.sub) {
			show(p, level+1);
		}
	}
}

packet_t decode(bitstream_t &bits) {
	packet_t packet;

	packet.version = bits.read(3);
	packet.id = bits.read(3);
	switch (packet.id) {
		case 4:
			packet.value = decode_literal(bits);
			break;
		default:
			packet.sub = decode_subpackets(bits);
			break;
	}

	return packet;
}

size_t decode_literal(bitstream_t &bits) {
	size_t literal = 0x00;

	// size_t nybbles = 1;
	while (bits.read(1)) {
		literal = (literal << 4) | bits.read(4);
		// nybbles++;
	}
	literal = (literal << 4) | bits.read(4);
	return literal;
}

vector<packet_t> decode_subpackets(bitstream_t &bits) {
	vector<packet_t> packets;

	if (bits.read(1)) {	// length type ID
		size_t packet_count = bits.read(11);
		while (packet_count--) {
			packets.push_back(decode(bits));
		}
	} else {
		size_t bit_count = bits.read(15);
		size_t current = bits.tell();
		while (bits.tell() < current+bit_count) {
			packets.push_back(decode(bits));
		}
	}

	return packets;
}

size_t eval(const packet_t &packet) {
	size_t result = 0;
	auto subs = packet.sub;

	switch (packet.id) {
		case 0:	// sum packets
			result = accumulate(subs.begin(), subs.end(), (size_t)0, [](size_t result, const packet_t &packet) {
				return result + eval(packet);
			});
			break;
		case 1:	// product packets
			result = accumulate(subs.begin(), subs.end(), (size_t)1, [](size_t result, const packet_t &packet) {
				return result * eval(packet);
			});
			break;
		case 2:	// min of packets
			result = accumulate(subs.begin(), subs.end(), (size_t)SIZE_MAX, [](size_t result, const packet_t &packet) {
				return min(result, eval(packet));
			});
			break;
		case 3:	// max packets
			result = accumulate(subs.begin(), subs.end(), (size_t)0, [](size_t result, const packet_t &packet) {
				return max(result, eval(packet));
			});
			break;
		case 4:	// literal
			result = packet.value;
			break;
		case 5:	// grater than 1 if a > b, otherwise 0
			assert(subs.size() == 2);
			result = (eval(subs[0]) > eval(subs[1])) ? 1 : 0;
			break;
		case 6:	// less than 1 if a < b, otherwise 0
			assert(subs.size() == 2);
			result = (eval(subs[0]) < eval(subs[1])) ? 1 : 0;
			break;
		case 7:	// equal; 1 if a == b, otherwise 0
			assert(subs.size() == 2);
			result = (eval(subs[0]) == eval(subs[1])) ? 1 : 0;
			break;
		default:
			cerr << "ERROR: Unknown packet id=" << packet.id << endl;
			break;
	}

	return result;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	std::function<size_t (const packet_t &)> version_sum;
	version_sum = [&version_sum](const packet_t &packet) {
		size_t result = packet.version;

		for (const auto &sub : packet.sub) {
			result += version_sum(sub);
		}
	
		return result;	
	};

	size_t result = 0;
	for (const auto &packet_str : data) {
		bitstream_t bits(packet_str);
		packet_t packet = decode(bits);
		
		auto local = version_sum(packet);
		// cout << local << "\t" << packet_str << endl;
		result += local;
	}

	return to_string(result);
}

//     161309575 too low --> truncated results
// 1675198555015 --> use size_t cast in accumulate!
const result_t part2(const data_t &data) {
	size_t result = 0;
	for (const auto &packet_str : data) {
		bitstream_t bits(packet_str);
		packet_t packet = decode(bits);

		auto local = eval(packet);
		// cout << local << "\t" << packet_str << endl;
		result += local;
	}

	return to_string(result);
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
