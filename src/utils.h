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
        class CharSequence {
        private:
            char *data_;
            size_t length_;
            size_t capacity_;
        public:
            const static size_t npos = -1;
        private:
            void resize(size_t new_capacity);

        public:
            CharSequence();

            CharSequence(const char *data, size_t offset, size_t length);

            CharSequence(const std::string &str);

            CharSequence(const char *c_str);

            CharSequence(const CharSequence &other);

            CharSequence(CharSequence &&other);

            ~CharSequence();

            CharSequence operator=(const CharSequence &other);

            bool operator==(const CharSequence &other);

            bool operator!=(const CharSequence &other);

            bool operator==(const std::string &other);

            bool operator!=(const std::string &other);

            bool operator==(const char *c_str);

            bool operator!=(const char *c_str);

            const char *data() const { return data_; }

            size_t length() const { return length_; }

            size_t size() { return length_; }

            size_t capacity() { return capacity_; }

            void append(const char *data, size_t offset, size_t length);

            CharSequence &append(size_t val);

            CharSequence &append(char c);

            CharSequence &append(const char *c_str);

            CharSequence &append(const CharSequence &sequence);

            CharSequence &append(const std::string &str);


            size_t find(char c) const;

            CharSequence &&sub_sequence(size_t l, size_t right = size_t(-1)) const;

        };

        std::string sha1digit(const CharSequence &char_seq);

        void split(std::vector<std::string> &vector, const std::string &src, char delim);

        uint32_t min_pow2_greater_than(uint32_t n);

        uint64_t min_pow2_greater_than(uint64_t n);

    }
}

#endif //DIT_UTILS_H
