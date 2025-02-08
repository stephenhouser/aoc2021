#include <getopt.h>     // getopt() argument processing
#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

#include <regex>		// regular expression
#include <vector>		// collectin
#include <string>		// strings
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <cstring>      // strtok, strdup

/*
	check -a answer.txt program

	reads answer.txt and checks against correct answers as listed in
	the answer.txt file

	for example:
	test.txt: 1 2
	input.txt: 0 0

	will run and check the output from program

          parse (    0.0268ms)
              0 (    0.0392ms)
              0 (    0.0038ms)
          total (    0.0698ms)
*/

using namespace std;

/* verbosity level; 0 = nothing extra, 1 = more... Set by command line. */
static int verbose = 0;
static bool show_time = false;
static int answer_width = 15;
static int time_width = 10;
static int time_precision = 4;

const vector<string> split_str(const string &str, const string &delim = ", =;") {
    vector<string> tokens;
    char *str_c { strdup(str.c_str()) };
    char *token { NULL };

    token = strtok(str_c, delim.c_str()); 
    while (token != NULL) { 
        tokens.push_back(std::string(token));  
        token = strtok(NULL, delim.c_str()); 
    }

    free(str_c);
    return tokens;
}

struct solution_t {
	string filename;
	string p1;
	string p2;

	solution_t(const string &line) {
		auto parts{split_str(line)};
		if (parts.size() > 1) {
			filename = parts[0];
			if (filename.ends_with(':')) {
				filename.pop_back();
			}

			if (parts.size() > 2) {
				p1 = parts[1];
				if (parts.size() > 2) {
					p2 = parts[2];
				}
			}
		}
	}
};

const vector<solution_t> read_solutions(const string &filename) {
	vector<solution_t> solutions;
	std::ifstream ifs(filename);
	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			solutions.push_back(solution_t(line));
		}
	}

	return solutions;
}

string run(const string &cmd, const string &arg = "") {
	string command{"./"};
	command.append(cmd);
	command.append(" ");
	command.append(arg);

	array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe) {
        throw runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

using result_t = pair<string, double>;
vector<result_t> parse_result(const string &output) {
	vector<result_t> results; 

	regex result_regex("\\s*(\\S+)\\s*\\(\\s*(\\S+)\\)");

	auto m_beg = sregex_iterator(output.begin(), output.end(), result_regex);
	auto m_end = sregex_iterator();
	for (regex_iterator i = m_beg; i != m_end; ++i) {
		smatch match = *i;
		results.push_back({match.str(1), atof(match.str(2).c_str())});
	}
	
	return results;
}


int main(int argc, char *argv[]) {
	const char *program = argv[1];
	if (argc < 2) {
		program = "solution";
	}

	auto solutions = read_solutions("answer.txt");

	for (const auto &solution : solutions) {
		auto results = parse_result(run(program, solution.filename));

		if (results[1].first == solution.p1) {
			cout << "p1=\033[1;97m*";
		} else {
			cout << "p1= ";
		}

		cout << setw(answer_width) << left << " " << results[1].first << "\033[0m";

		std::cout << " ("
			<< std::setw(time_width) << std::fixed << std::right 
			<< std::setprecision(time_precision)
			<< results[1].second << "ms)";


		cout << "\t";

		if (results[2].first == solution.p2) {
			cout << "p2=\033[1;93m*";
		} else {
			cout << "p2= ";
		}

		cout << setw(answer_width) << left << " " << results[2].first << "\033[0m";

		std::cout << " ("
			<< std::setw(time_width) << std::fixed << std::right 
			<< std::setprecision(time_precision)
			<< results[2].second << "ms)";

		cout << endl;
	}
}
