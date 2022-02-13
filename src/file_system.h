//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_FILE_SYSTEM_H
#define DIT_FILE_SYSTEM_H

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/filesystem/operations.hpp>
#include <zlib.h>
#include <fstream>
#include "utils.h"
#include "exceptions.h"

namespace dit {
    namespace boost_fs = boost::filesystem;
    namespace fs {

        extern boost_fs::path REPOSITORY_ROOT;
        extern const boost_fs::path REPOSITORY_INTERNAL_PATH;
        extern const boost_fs::path OBJECTS;
        extern const boost_fs::path REFS;
        extern const boost_fs::path HEADS;
        extern const boost_fs::path HEAD;
        extern const boost_fs::path INDEX;
        extern const boost_fs::path CONFIG;
        extern const boost_fs::path MASTER;

        boost_fs::path generate_path(const std::string &sha1);

        boost_fs::path find_repository(const boost_fs::path &target_path);

        bool configure_repository_root();

        bool file_read(const boost::filesystem::path &file_path, std::string &read_result);

        bool file_write(const boost::filesystem::path &file_path, const std::string &write_content);

        size_t recursive_travel(boost_fs::path &dir, std::vector<boost_fs::path> &paths);

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
