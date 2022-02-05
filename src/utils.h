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

namespace dit {
    namespace utils {
        std::string sha1digit(const std::string &str);

        void split(std::vector<std::string> &vector, const std::string &src, char delim);


    }
}

#endif //DIT_UTILS_H
