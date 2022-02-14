//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        void cmd_diff(const std::vector<std::string> &args) {
            std::string str_file_path;
            std::string base_commit_id;
            std::string target_commit_id;
            program_options::options_description desc("options");
            desc.add_options()
                    ("help", "give you all useful information")
                    ("path", program_options::value<std::string>(&str_file_path),
                     "file path be compared between base and target")
                    ("base,b", program_options::value<std::string>(&base_commit_id),
                     "base commit id")
                    ("target,t", program_options::value<std::string>(&target_commit_id),
                     "target commit id");
            program_options::variables_map vm;
            auto parsed = program_options::command_line_parser(args).options(desc).run();
            program_options::store(parsed, vm);
            program_options::notify(vm);

            auto &global_index = index::Index::instance();

            index::CommitIndex base_commit_index = index::CommitIndex(base_commit_id);
            index::CommitIndex target_commit_index = index::CommitIndex(target_commit_id);

            auto diffs_per_line = base_commit_index.compare_to(target_commit_index,
                                                                               boost_fs::path(str_file_path));

            for(auto &pair : diffs_per_line){
                std::string header = "\033[22;31m-";
                if (pair.second == index::Status::ADD){
                    header = "\033[22;32m+";
                }else if(pair.second == index::Status::SAME){
                    header = " ";
                }
                std::cout<<header<<pair.first<<"\033[0m"<<std::endl;
            }


        }
    }
}


