//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        void cmd_log(const std::vector<std::string> &args) {
            auto commit_id = refs::read_head();
            std::ostringstream oss;
            while (commit_id != ROOT_COMMIT_ID) {
                objects::CommitObject commit;
                commit.read(commit_id);
                commit.log(oss);
                auto &parent = commit.get_parents();
                if(parent.size() == 0) break;
                commit_id = parent[0];
            }
            std::cout << oss.str();
        }
    }
}


