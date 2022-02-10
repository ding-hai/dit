//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_EXCEPTIONS_H
#define DIT_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>

namespace dit {
    namespace exceptions {
        class MalformedException : public std::exception {
        private:
            const char *what_;
        public:
            MalformedException(const char *what) : what_(what) {}

            const char *what() {
                return what_;
            }

        };

        class RepositoryException : public std::exception {
        private:
            const char *what_;
        public:
            RepositoryException(const char *what) : what_(what) {}

            const char *what() {
                return what_;
            }

        };

        class FileException : public std::exception {
        private:
            std::string what_;
        public:
            FileException(std::string what) : what_(std::move(what)) {}

            const char *what() {
                return what_.c_str();
            }

        };
    }
}

#endif //DIT_EXCEPTIONS_H
