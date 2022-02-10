//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        std::string ROOT_COMMIT_ID("0000000000000000000000000000000000000000");

        void cmd_commit(const std::vector<std::string> &args) {
            std::string commit_msg;
            std::vector<boost_fs::path> file_paths;
            namespace program_options = boost::program_options;

            program_options::options_description desc("options");
            desc.add_options()
                    ("help", "give you all useful information")
                    ("m", program_options::value<std::string>(&commit_msg),
                     "commit message");
            program_options::variables_map vm;
            auto parsed = program_options::command_line_parser(args).options(desc).run();
            program_options::store(parsed, vm);
            program_options::notify(vm);

            if (!vm.count("m") || vm.count("help")) {
                std::cout << "usage: dit commit -m \"commit message\"" << std::endl
                          << std::endl
                          << desc << std::endl;
            }

            auto &index = dit::index::Index::instance();
            auto root_tree_sha1 = index.to_tree_object();
            dit::objects::CommitObject commit;
            commit.set_root_tree(root_tree_sha1);
            //todo: filled with configuration
            dit::objects::CommitObject::User author("dinghai", "dhairoot@126.com");
            dit::objects::CommitObject::User committer("dinghai", "dhairoot@126.com");

            commit.add_parent(refs::read_head());
            commit.set_committer(committer);
            commit.set_author(author);
            commit.set_timestamp(std::to_string(std::time(nullptr)));
            commit.set_commit_msg(commit_msg);
            auto commit_sha1 = commit.write();
            refs::write_head(commit_sha1);
        }
    }
}


