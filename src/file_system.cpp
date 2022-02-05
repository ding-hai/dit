//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "file_system.h"

namespace dit {
    namespace fs {
        const boost::filesystem::path REPOSITORY_ROOT("./.dit/");
        const boost::filesystem::path OBJECTS("objects");

        std::string ObjectWriter::write(const std::string &content) {
            auto sha1 = utils::sha1digit(content);
            auto path = generate_path(sha1);
            boost::filesystem::create_directories(path.parent_path());
            gzFile gz_file = gzopen(path.string().c_str(), "wb");
            gzwrite(gz_file, content.c_str(), content.length());
            gzclose(gz_file);
            return sha1;
        }

        std::string ObjectReader::read(const std::string &sha1) {
            auto path = generate_path(sha1);
            gzFile gz_file = gzopen(path.string().c_str(), "rb");
            std::string file_content;
            char buffer[100 * 1024];
            int length = 0;
            while ((length = gzread(gz_file, buffer, sizeof buffer)) > 0) {
                file_content.append(buffer, length);
            }
            return file_content;
        }

        boost::filesystem::path generate_path(const std::string &sha1) {
            return REPOSITORY_ROOT / OBJECTS / sha1.substr(0, 2) / sha1.substr(2);
        }
    }
}
