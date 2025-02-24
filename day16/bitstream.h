#if !defined(BITSTREAM_H)
#define BITSTREAM_H

#include <iostream>		// cout
#include <cstdint>		// uint8_t
#include <string>		// strings
#include <vector>		// collections
#include <functional>

static uint8_t decode_hex(const char hex) {
	if ('0' <= hex && hex <= '9') {
		return (uint8_t)(hex - '0');
	} else if ('A' <= hex && hex <= 'F') {
		return (uint8_t)((hex - 'A') + 10);
	}

	std::cerr << "ERROR: Decoding hex " << std::hex 
			  << " is out of range" << std::endl;
	return 0;
}

static std::vector<uint8_t> decode_hex(const std::string &str) {
	std::vector<uint8_t> bytes;

	for (size_t i = 0; i < str.size(); i += 2) {
		uint8_t high = decode_hex(str[i]);
		uint8_t low = (i+1 < str.size()) ? decode_hex(str[i+1]) : 0x00;
		bytes.push_back(high << 4 | low);
	}

	return bytes;
}

class bitstream_t {
	private:
		std::vector<uint8_t> buffer;
		size_t pos;

	public:
		bitstream_t(const std::string &hex) : buffer(decode_hex(hex)), pos(0) {
		}

		size_t tell() const {
			return pos;
		}

		size_t read(const size_t bits) {
			auto s_byte = pos / 8;	// which byte do we start in
			auto s_bit  = pos % 8;	// which bit in that byte
			auto e_byte = (pos+bits) / 8;
			auto e_bit  = (pos+bits) % 8;

			if (bits > 16) {
				std::cerr << "ERROR: Too many bits! " << bits << std::endl;
			}
			pos += bits;

			size_t full = 0x00;
			for (size_t b = s_byte; b <= e_byte; b++) {
				full = (full << 8) | buffer[b];
				s_bit += 8;
			}

			size_t mask = ~(~0u << bits);
			size_t result = full >> (8 - e_bit);		
			return result & mask;
		}
};

#endif