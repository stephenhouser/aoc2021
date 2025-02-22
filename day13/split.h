#if !defined(SPLIT_H)
#define SPLIT_H

#include <vector>		// std::vector
#include <cstring>      // strtok, strdup

/*
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

const std::vector<int> split_int(const std::string &str, const std::string &delim = ", =;") {
    std::vector<int> result;

    size_t start = 0;
    size_t end = start;
    while (start != std::string::npos && end != std::string::npos) {
        size_t end = str.find_first_of(delim, start);

        if (std::isdigit(str[start]) || str[start] == '-') {
            result.push_back(std::strtol(str.c_str() + start));
        }

        start = str.find_first_not_of(delim, end);
    }

    return result;
}
    */


const std::vector<std::string> split_str(const std::string &str, const std::string &delim = ", =;") {
    std::vector<std::string> tokens;
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

const std::vector<int> split_int(const std::string &str, const std::string &delims = ", =;") {
    std::vector<int> tokens;
    char *str_c { strdup(str.c_str()) };
    char *token { NULL };

    token = strtok(str_c, delims.c_str()); 
    while (token != NULL) { 
        if (isdigit(token[0])||token[0]=='-') {
            tokens.push_back(atoi(token));
        }
        token = strtok(NULL, delims.c_str()); 
    }

    free(str_c);
    return tokens;
}

const std::vector<size_t> split_size_t(const std::string &str, const std::string &delims = ", =;") {
    std::vector<size_t> tokens;
    char *str_c { strdup(str.c_str()) };
    char *token { NULL };

    token = strtok(str_c, delims.c_str()); 
    while (token != NULL) { 
        if (isdigit(token[0])) {
            tokens.push_back((size_t)atoi(token));
        }
        token = strtok(NULL, delims.c_str()); 
    }

    free(str_c);
    return tokens;
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
