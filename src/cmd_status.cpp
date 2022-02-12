//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        void cmd_status(const std::vector<std::string> &args) {
            // compare between working directory and index
            index::WorkingIndex working_index;
            auto &global_index = index::Index::instance();
            auto diffs_between_index_and_working_dir = working_index.compare_to(global_index);
            std::cout<<"diff between index and working dir:"<<std::endl;
            for (auto &pair: diffs_between_index_and_working_dir) {
                std::cout<<pair.first.generic_string()<<" status: "<<pair.second<<std::endl;
            }
            // compare index and commit
            auto head_commit_id = refs::read_head();
            index::CommitIndex commit_index(head_commit_id);
            auto diffs_between_commit_and_index = global_index.compare_to(commit_index);
            std::cout<<"diff between commit and index:"<<std::endl;
            for (auto &pair: diffs_between_commit_and_index) {
                std::cout<<pair.first.generic_string()<<" status: "<<pair.second<<std::endl;
            }
        }
    }
}


