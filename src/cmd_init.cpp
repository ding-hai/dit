//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/10.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        void cmd_init(const std::vector<std::string> &args) {
            if (args.size() != 1) return;
            const std::string &arg = args[0];
            boost_fs::path target_path;
            if (arg.empty() || arg == ".") {
                target_path = boost_fs::current_path();
            } else {
                target_path = std::move(boost_fs::path(arg));
            }

            auto absolute_path = boost_fs::absolute(target_path);
            auto repo_root = fs::find_repository(absolute_path);
            if (!repo_root.empty()) {
                std::cout << "repo exists in " << repo_root.generic_string() << std::endl;
                return;
            }
            fs::REPOSITORY_ROOT = absolute_path;
            const auto base = fs::REPOSITORY_ROOT / fs::REPOSITORY_INTERNAL_PATH;
            boost_fs::create_directories(base / fs::OBJECTS);
            boost_fs::create_directories(base / fs::REFS / fs::HEADS);

            auto root_commit_id = "0000000000000000000000000000000000000000";
            auto ref_head = "ref: refs/heads/master";
            fs::file_write(base / fs::HEAD, ref_head);
            fs::file_write(base / fs::REFS / fs::HEADS / fs::MASTER, root_commit_id);
        }


    }
}