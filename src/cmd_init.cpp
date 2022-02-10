//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/10.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        bool cmd_init(const std::string &arg) {
            boost_fs::path target_path;
            if (arg.empty() || arg == ".") {
                target_path = boost_fs::current_path();
            } else {
                target_path = std::move(boost_fs::path(arg));
            }

            auto &&absolute_path = boost_fs::absolute(target_path);
            auto repo_root = fs::find_repository(absolute_path);
            if (!repo_root.empty()) {
                std::cout << "repo exists in " << repo_root.generic_string() << std::endl;
                return false;
            }
            fs::REPOSITORY_ROOT = absolute_path;
            const auto base = fs::REPOSITORY_ROOT / fs::REPOSITORY_INTERNAL_PATH;
            boost_fs::create_directories(base / fs::OBJECTS);
            boost_fs::create_directories(base / fs::REFS / fs::HEADS);

            auto root_commit_id = "0000000000000000000000000000000000000000";
            fs::file_write(base / fs::HEAD, root_commit_id);
            fs::file_write(base / fs::REFS / fs::HEADS / fs::MASTER, root_commit_id);
            return true;
        }


    }
}