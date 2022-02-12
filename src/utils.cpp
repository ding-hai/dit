//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "utils.h"

namespace dit {
    namespace utils {

        std::string ROOT_COMMIT_ID("0000000000000000000000000000000000000000");

        std::string sha1digit(const std::string &content) {
            boost::uuids::detail::sha1 sha1;
            boost::uuids::detail::sha1::digest_type sha1_digit_value;
            sha1.process_bytes(content.data(), content.length());
            sha1.get_digest(sha1_digit_value);

            return sha1digit_to_string(sha1_digit_value);
        }


        void left_trim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
        }

        void right_trim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        std::string sha1digit_to_string(boost::uuids::detail::sha1::digest_type &sha1_digit_value) {
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

        uint32_t min_pow2_greater_than(uint32_t n) {
            if (n < 1) return 1;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            return n + 1;
        }

        uint64_t min_pow2_greater_than(uint64_t n) {
            if (n < 1) return 1;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            n |= n >> 32;
            return n + 1;
        }
    }
}