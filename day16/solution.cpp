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

using namespace std;

using data_t = vector<string>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

uint8_t decode_hex(const char hex) {
	if ('0' <= hex && hex <= '9') {
		return (uint8_t)(hex - '0');
	} else if ('A' <= hex && hex <= 'F') {
		return (uint8_t)((hex - 'A') + 10);
	}

	cerr << "ERROR: Decoding hex " << hex << " is out of range" << endl;
	return 0;
}

vector<uint8_t> decode_hex(const string &str) {
	vector<uint8_t> bytes;

	for (size_t i = 0; i < str.size(); i += 2) {
		uint8_t high = decode_hex(str[i]);
		uint8_t low = (i+1 < str.size()) ? decode_hex(str[i+1]) : 0x00;
		bytes.push_back(high << 4 | low);
	}

	return bytes;
}

class bitstream_t {
	private:
		vector<uint8_t> buffer;
		size_t pos;

	public:
		bitstream_t(const string &hex) : buffer(decode_hex(hex)), pos(0) {
		}

		size_t tell() const {
			return pos;
		}

		size_t read(const size_t bits) {
			auto s_byte = pos / 8;	// which byte do we start in
			auto s_bit  = pos % 8;	// which bit in that byte
			auto e_byte = (pos+bits) / 8;
			auto e_bit  = (pos+bits) % 8;

			pos += bits;

			size_t full = 0x00;
			for (size_t b = s_byte; b <= e_byte; b++) {
				full = (full << 8) | buffer[b];
				s_bit += 8;
			}

			size_t mask = ~(~0u << bits);
			size_t result = full >> (8 - e_bit);		
			// cout << "mask=" << hex << mask << " res=" << result << " e_b=" << e_bit << endl; 
			return result & mask;
		}
};

struct packet_t {
	size_t version = 0x00;
	size_t id = 0x00;
	size_t value = 0x00;
	vector<packet_t> sub = {};
};

packet_t decode(bitstream_t &bits);
size_t decode_literal(bitstream_t &bits);
vector<packet_t> decode_subpackets(bitstream_t &bits);

void show(const packet_t &packet, const int level = 0) {
	for (int i = 0; i < level; i++) {
		cout << "\t";
	}

	cout << "version=" << packet.version << " id=" << packet.id;
	switch (packet.id) {
		case 4:
			cout << " literal=" << packet.value << endl;
			break;
		default:
			cout << endl;
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

	while (bits.read(1)) {
		literal = (literal << 4) | bits.read(4);
	}
	literal = (literal << 4) | bits.read(4);

	return literal;
}

vector<packet_t> decode_subpackets(bitstream_t &bits) {
	vector<packet_t> packets;

	// length type ID
	if (bits.read(1)) {
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


// void traverse(const packet_t &packet, const std::function<void(const packet_t &)>& callback) {
// 	switch (packet.id) {
// 		case 4:
// 			callback(packet);
// 			break;
// 		default:
// 			for (const auto &sub : packet.sub) {
// 				traverse(sub, callback);
// 			}
// 	}
// }

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
		
		result += version_sum(packet);
	}

	return to_string(result);
}

const result_t part2([[maybe_unused]] const data_t &data) {
	return to_string(0);
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
