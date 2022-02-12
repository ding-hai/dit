//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {
        bool reset(const std::string &reset_id) {
            index::CommitIndex *commit_index = nullptr;
            auto id = refs::read_head();
            while (id != utils::ROOT_COMMIT_ID) {
                objects::CommitObject commit;
                commit.read(id);
                if (id == reset_id) {
                    commit_index = new index::CommitIndex(commit);
                }
                auto &parent = commit.get_parents();
                if (parent.empty()) break;
                id = parent[0];
            }

            if (commit_index == nullptr) {
                std::cout << "error :" << reset_id << " doesn't exist in this branch" << std::endl;
                return false;
            }
            auto &global_index = index::Index::instance();
            commit_index->swap_to(global_index);
            commit_index->recover_to_working_dir();
            global_index.save();
            refs::write_head(reset_id);

            delete commit_index;
            return true;
        }

        void cmd_reset(const std::vector<std::string> &args) {
            std::string reset_id;
            std::vector<boost_fs::path> file_paths;
            namespace program_options = boost::program_options;

            program_options::options_description desc("options");
            desc.add_options()
                    ("help", "give you all useful information")
                    ("id", program_options::value<std::string>(&reset_id),
                     "reset to history id");
            program_options::variables_map vm;
            auto parsed = program_options::command_line_parser(args).options(desc).run();
            program_options::store(parsed, vm);
            program_options::notify(vm);

            if (!vm.count("id") || vm.count("help")) {
                std::cout << "usage: dit reset --id \"history commit id\"" << std::endl
                          << std::endl
                          << desc << std::endl;
            }

            reset(reset_id);
        }
    }
}


