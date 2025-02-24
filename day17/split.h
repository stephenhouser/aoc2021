#if !defined(SPLIT_H)
#define SPLIT_H

#include <vector>		// std::vector
#include <cstring>      // strtok, strdup

const std::vector<std::string> split(const std::string &str, const std::string &delim = ", =;") {
    std::vector<std::string> result;

    size_t start = 0;
    size_t end = start;
    while (start != std::string::npos && end != std::string::npos) {
        size_t end = str.find_first_of(delim, start);

        result.push_back(str.substr(start, (end == std::string::npos) ? std::string::npos : end - start));

        start = str.find_first_not_of(delim, end);
    }

    return result;
}

const std::vector<int> split_int(const std::string &str, const std::string &delim = ", =;.") {
    std::vector<int> result;

    size_t start = 0;
    size_t end = start;
    while (start != std::string::npos && end != std::string::npos) {
        size_t end = str.find_first_of(delim, start);

        if (std::isdigit(str[start]) || str[start] == '-') {
            result.push_back(std::stoi(str.substr(start)));
        }

        start = str.find_first_not_of(delim, end);
    }

    return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> &v) {
    for (auto it = v.begin(); it != v.end(); it++) {
        if (it != v.begin()) {
            os << ",";
        }
        os << *it;
    }
	return os;
}

#endif
