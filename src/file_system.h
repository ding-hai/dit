//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_FILE_SYSTEM_H
#define DIT_FILE_SYSTEM_H

#include <boost/filesystem/path.hpp>
#include <zlib.h>
#include "utils.h"

namespace dit {
    namespace fs {
        boost::filesystem::path generate_path(const std::string &sha1);

        class ObjectWriter {
        public:
            virtual std::string
            write(const std::string &content);
        };

        class ObjectReader {
        public:
            std::string read(const std::string &sha1);
        };
    }
}
#endif //DIT_FILE_SYSTEM_H
