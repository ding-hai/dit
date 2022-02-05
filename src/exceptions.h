//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_EXCEPTIONS_H
#define DIT_EXCEPTIONS_H

#include <exception>

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
    }
}

#endif //DIT_EXCEPTIONS_H
