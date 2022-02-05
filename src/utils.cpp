//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "utils.h"

namespace dit {
    namespace utils {
        std::string sha1digit(const std::string &str) {
            uint32_t sha1_digit_value[5];
            boost::uuids::detail::sha1 sha1;
            sha1.process_bytes(str.c_str(), str.length());
            sha1.get_digest(sha1_digit_value);
            std::ostringstream oss;
            for (auto &v: sha1_digit_value) {
                oss << std::setw(8) << std::setfill('0') << std::hex << v;
            }
            return oss.str();
        }

        void split(std::vector<std::string> &vector, const std::string &src, char delim) {
            std::istringstream iss(src);
            std::string tmp;
            while (std::getline(iss, tmp, delim)) {
                vector.emplace_back(std::move(tmp));
            }
        }


    }
}