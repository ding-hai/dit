//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "file_system.h"

namespace dit {
    namespace fs {
        boost_fs::path REPOSITORY_ROOT = boost_fs::current_path();
        const boost_fs::path REPOSITORY_INTERNAL_PATH(".dit");
        const boost_fs::path OBJECTS("objects");
        const boost_fs::path REFS("refs");
        const boost_fs::path HEADS("heads");
        const boost_fs::path HEAD("HEAD");
        const boost_fs::path INDEX("index");
        const boost_fs::path CONFIG("config");
        const boost_fs::path MASTER("master");

        boost_fs::path find_repository(const boost_fs::path &target_path) {
            auto tmp = target_path;
            while (!tmp.empty() && !boost_fs::is_directory(tmp / REPOSITORY_INTERNAL_PATH)) {
                tmp = tmp.parent_path();
            }
            return tmp;
        }


        bool configure_repository_root() {
            auto current_path = boost_fs::current_path();
            current_path = find_repository(current_path);
            if (current_path.empty())
                return false;
            REPOSITORY_ROOT = current_path;
            return true;
        }

        std::string ObjectWriter::write(const std::string &content) {
            auto sha1_string = utils::sha1digit(content);
            auto path = generate_path(sha1_string);
            boost::filesystem::create_directories(path.parent_path());
            gzFile gz_file = gzopen(path.string().c_str(), "wb");
            gzwrite(gz_file, content.data(), content.length());
            gzflush(gz_file, Z_FINISH);
            gzclose(gz_file);
            return sha1_string;
        }

        std::string ObjectReader::read(const std::string &sha1) {
            auto path = generate_path(sha1);
            gzFile gz_file = gzopen(path.string().c_str(), "rb");
            std::string file_content;
            char buffer[100 * 1024];
            int length = 0;
            while ((length = gzread(gz_file, buffer, sizeof buffer)) > 0) {
                file_content.append(buffer, 0, length);
            }
            return file_content;
        }

        boost::filesystem::path generate_path(const std::string &sha1) {
            return REPOSITORY_ROOT / REPOSITORY_INTERNAL_PATH / OBJECTS / sha1.substr(0, 2) / sha1.substr(2);
        }

        bool file_read(const boost::filesystem::path &file_path, std::string &read_result) {
            if(!boost::filesystem::exists(file_path)) return false;
            auto file_size = boost::filesystem::file_size(file_path);
            // 5M
            if (file_size > (5 << 20))
                return false;
            if(file_size == 0)
                return false;
            char buffer[file_size];
            boost::filesystem::ifstream file(file_path, std::ios::in | std::ios::binary);
            file.read(buffer, file_size);
            read_result.clear();
            read_result.append(buffer, buffer + file_size);
            return true;
        }

        bool file_write(const boost::filesystem::path &file_path, const std::string &write_content) {
            boost::filesystem::ofstream file(file_path, std::ios::out);
            file << write_content;
            file.flush();
            file.close();
            return true;
        }

        // dir must be an absolute path
        size_t recursive_travel(boost_fs::path &dir, std::vector<boost_fs::path> &paths) {
            boost_fs::recursive_directory_iterator iterator(dir);
            boost_fs::recursive_directory_iterator end;
            size_t num = 0;
            for (;iterator != end; ++iterator) {
                boost_fs::path path(*iterator);
                auto &&dit_path = REPOSITORY_ROOT / REPOSITORY_INTERNAL_PATH;
                if(path.generic_string().find(dit_path.generic_string()) == 0)
                    continue;
                if(boost_fs::is_directory(path))
                    continue;
                paths.push_back(path);
                num++;

            }
            return num;
        }
    }
}
