//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_UTILS_H
#define DIT_UTILS_H

#include <iomanip>
#include <sstream>
#include <vector>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/filesystem.hpp>

namespace std {
    template<>
    struct hash<boost::filesystem::path> {
        size_t operator()(const boost::filesystem::path &path) const {
            return boost::filesystem::hash_value(path);
        }
    };
}

namespace dit {
    namespace utils {


        void left_trim(std::string &s);

        void right_trim(std::string &s);

        std::string sha1digit_to_string(boost::uuids::detail::sha1::digest_type &sha1_digit_value);

        std::string sha1digit(const std::string &content);

        void split(std::vector<std::string> &vector, const std::string &src, char delim);

        uint32_t min_pow2_greater_than(uint32_t n);

        uint64_t min_pow2_greater_than(uint64_t n);

    }
}

#endif //DIT_UTILS_H
