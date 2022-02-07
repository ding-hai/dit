//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "file_system.h"

namespace dit {
    namespace fs {
        const boost::filesystem::path REPOSITORY_ROOT("./.dit/");
        const boost::filesystem::path OBJECTS("objects");

        std::string
        ObjectWriter::write(const utils::CharSequence &content, boost::uuids::detail::sha1::digest_type &sha1) {
            utils::sha1digit(content, sha1);
            auto sha1_string = utils::sha1digit_to_string(sha1);
            auto path = generate_path(sha1_string);
            boost::filesystem::create_directories(path.parent_path());
            gzFile gz_file = gzopen(path.string().c_str(), "wb");
            gzwrite(gz_file, content.data(), content.length());
            gzclose(gz_file);
            return sha1_string;
        }

        utils::CharSequence ObjectReader::read(const std::string &sha1) {
            auto path = generate_path(sha1);
            gzFile gz_file = gzopen(path.string().c_str(), "rb");
            utils::CharSequence file_content;
            uint8_t buffer[100 * 1024];
            int length = 0;
            while ((length = gzread(gz_file, buffer, sizeof buffer)) > 0) {
                file_content.append(buffer, 0, length);
            }
            return file_content;
        }

        boost::filesystem::path generate_path(const std::string &sha1) {
            return REPOSITORY_ROOT / OBJECTS / sha1.substr(0, 2) / sha1.substr(2);
        }
    }
}
