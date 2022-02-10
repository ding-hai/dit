//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/10.
//

#include "refs.h"

namespace dit {
    namespace refs {

        boost_fs::path get_ref_path() {
            static boost_fs::path default_ref(
                    fs::REPOSITORY_ROOT / fs::REPOSITORY_INTERNAL_PATH / fs::HEADS / fs::REFS / fs::MASTER);
            std::string ref_info;
            auto ok = dit::fs::file_read(fs::REPOSITORY_ROOT / fs::REPOSITORY_INTERNAL_PATH / fs::HEAD, ref_info);
            if (!ok)
                return default_ref;

            std::vector<std::string> items;
            utils::split(items, ref_info, ':');
            if (items.size() < 2)
                return default_ref;

            auto &ref_path = items[1];
            utils::left_trim(ref_path);
            utils::right_trim(ref_path);
            return fs::REPOSITORY_ROOT / fs::REPOSITORY_INTERNAL_PATH / ref_path;
        }

        std::string read_head() {
            auto &&path = get_ref_path();
            std::string commit_id;
            fs::file_read(path, commit_id);
            return commit_id;
        }

        bool write_head(const std::string &commit_id) {
            auto &&path = get_ref_path();
            return fs::file_write(path, commit_id);
        }
    }
}