//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "utils.h"

namespace dit {
    namespace utils {
        std::string sha1digit(const CharSequence &char_seq) {
            uint32_t sha1_digit_value[5];
            boost::uuids::detail::sha1 sha1;
            sha1.process_bytes(char_seq.data(), char_seq.length());
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

        CharSequence::CharSequence() {
            length_ = 0;
            capacity_ = 0;
            data_ = nullptr;
        }

        CharSequence::CharSequence(const char *data, size_t offset, size_t length) {
            capacity_ = min_pow2_greater_than((uint64_t) length);
            assert(capacity_ > length);
            length_ = length;
            data_ = new char[capacity_]();
            memcpy(data_, data, length);
            data_[length] = '\0';
        }

        CharSequence::CharSequence(const char *c_str) : CharSequence(c_str, 0, strlen(c_str)) {}

        CharSequence::CharSequence(const std::string &str) : CharSequence(str.c_str(), 0, str.size()) {}

        CharSequence::CharSequence(const CharSequence &other) {
            if (this == &other) return;
            capacity_ = other.capacity_;
            length_ = other.length_;
            data_ = new char[capacity_];
            memcpy(data_, other.data_, length_);
        }

        CharSequence::CharSequence(CharSequence &&other) {
            if (this == &other) return;
            this->data_ = other.data_;
            this->length_ = other.length_;
            this->capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.length_ = 0;
            other.capacity_ = 0;
        }

        CharSequence::~CharSequence() {
            if (data_) {
                delete[] data_;
                data_ = nullptr;
            }
        }

        CharSequence CharSequence::operator=(const CharSequence &other) {
            CharSequence new_char_sequence(other);
            return std::move(new_char_sequence);
        }

        bool CharSequence::operator==(const CharSequence &other) {
            if (this->length_ != other.length_) return false;
            int cmp = memcmp(this->data_, other.data_, this->length_);
            return cmp == 0;
        }

        bool CharSequence::operator!=(const CharSequence &other) {
            return !((*this) == other);
        }

        bool CharSequence::operator==(const std::string &other) {
            return operator==(other.c_str());
        }

        bool CharSequence::operator!=(const std::string &other) {
            return operator!=(other.c_str());
        }

        bool CharSequence::operator==(const char *c_str) {
            if (this->length_ != strlen(c_str)) return false;
            int cmp = memcmp(this->data_, c_str, this->length_);
            return cmp == 0;
        }

        bool CharSequence::operator!=(const char *c_str) {
            return !((*this) == c_str);
        }

        size_t CharSequence::find(char c) const {
            for (size_t i = 0; i < length_; i++) {
                if (c == data_[i]) return i;
            }
            return npos;
        }

        void CharSequence::append(const char *data, size_t offset, size_t length) {
            size_t new_size = length + length_;
            if (data_ == nullptr || new_size >= capacity_) {
                size_t new_capacity = capacity_ << 1;
                resize(new_capacity);
            }
            memcpy(data_ + length_, data + offset, length);
            data_[new_size] = '\0';
            length_ = new_size;
        }

        CharSequence &CharSequence::append(char c) {
            append(&c, 0, 1);
            return *this;
        }

        CharSequence &CharSequence::append(size_t val) {
            auto str = std::to_string(val);
            return append(str);
        }

        CharSequence &CharSequence::append(const std::string &str) {
            append(str.c_str(), 0, str.length());
            return *this;
        }

        CharSequence &CharSequence::append(const char *c_str) {
            append(c_str, 0, strlen(c_str));
            return *this;
        }

        CharSequence &CharSequence::append(const CharSequence &sequence) {
            append(sequence.data(), 0, sequence.length_);
            return *this;
        }

        void CharSequence::resize(size_t new_capacity) {
            char *new_data = new char[new_capacity]();
            if (data_ != nullptr) {
                memcpy(new_data, data_, length_);
                delete[] data_;
            }
            data_ = new_data;
            capacity_ = new_capacity;
        }

        CharSequence &&CharSequence::sub_sequence(size_t left, size_t right) const {
            right = std::min(right, length_);
            CharSequence new_sequence(data_, left, right - left);
            return std::move(new_sequence);
        }

    }
}