//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {
        bool rm_one_file(const boost::filesystem::path &file_path, index::CommitIndex &commit_index,
                         index::WorkingIndex &working_index, bool force) {
            auto relative_path = boost_fs::relative(file_path, fs::REPOSITORY_ROOT);
            std::string msg = relative_path.generic_string();
            if (!boost_fs::exists(file_path) || boost_fs::is_directory(file_path)) {
                msg.append(" is directory or not exist");
                return false;
            }

            auto &global_index = index::Index::instance();
            const auto &sha1_of_index = global_index.sha1_of_path(relative_path);
            const auto &sha1_of_commit = commit_index.sha1_of_path(relative_path);
            const auto &sha1_of_working_dir = global_index.sha1_of_path(relative_path);

            if (!force && (sha1_of_index != sha1_of_commit || sha1_of_index != sha1_of_working_dir)) {
                std::cout << file_path.generic_string()
                          << ": has changes among working area ,staged area and commit area" << std::endl;
                return false;
            }

            boost_fs::remove(relative_path);
            global_index.remove(relative_path);
            return true;
        }


        void cmd_rm(const std::vector<std::string> &args) {
            std::vector<std::string> str_file_paths;
            std::vector<boost_fs::path> file_paths;
            namespace program_options = boost::program_options;

            program_options::options_description desc("options");
            desc.add_options()
                    ("help", "give you all useful information")
                    ("path", program_options::value<std::vector<std::string>>(&str_file_paths),
                     "path list to add to index")
                    ("force,f", program_options::bool_switch()->default_value(false), "force remove");
            program_options::positional_options_description p;
            p.add("path", -1);
            program_options::variables_map vm;
            auto parsed = program_options::command_line_parser(args).options(desc).positional(p).run();
            program_options::store(parsed, vm);
            program_options::notify(vm);

            if (!vm.count("path") || vm.count("help")) {
                std::cout << "usage: dit add <path1> [<path2> ...]" << std::endl
                          << std::endl
                          << desc << std::endl;
            }

            bool force = vm["force"].as<bool>();

            for (auto &str_file_path: str_file_paths) {
                if (str_file_path == "*") {
                    file_paths.clear();
                    fs::recursive_travel(fs::REPOSITORY_ROOT, file_paths);
                    break;
                } else {
                    auto path = boost_fs::path(str_file_path);
                    auto absolute_path = boost_fs::absolute(path);
                    if (!boost_fs::exists(absolute_path)) continue;
                    if (boost_fs::is_directory(absolute_path)) {
                        fs::recursive_travel(absolute_path, file_paths);
                    } else {
                        file_paths.push_back(absolute_path);
                    }
                }
            }

            std::sort(file_paths.begin(), file_paths.end());
            file_paths.erase(std::unique(file_paths.begin(), file_paths.end()), file_paths.end());

            auto head = refs::read_head();
            auto &global_index = index::Index::instance();
            index::CommitIndex commit_index(head);
            index::WorkingIndex working_index;

            for (auto &file_path: file_paths) {
                rm_one_file(file_path, commit_index, working_index, force);
            }
            global_index.save();
        }
    }
}


