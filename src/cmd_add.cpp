//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {
        const std::string &add_one_file(const boost::filesystem::path &file_path) {
            if (!boost_fs::exists(file_path) || boost_fs::is_directory(file_path)) {
                std::string msg = file_path.generic_string();
                msg.append(" is directory or not exist");
                throw exceptions::FileException(msg);
            }
            std::string file_content;
            if (!fs::file_read(file_path, file_content)) {
                throw exceptions::FileException("file read exception: file size > 5MB or file not found");
            }
            objects::BlobObject blob(file_content);
            return blob.write();
        }


        void cmd_add(const std::vector<std::string> &args) {
            std::vector<std::string> str_file_paths;
            std::vector<boost_fs::path> file_paths;
            namespace program_options = boost::program_options;

            program_options::options_description desc("options");
            desc.add_options()
                    ("help", "give you all useful information")
                    ("path", program_options::value<std::vector<std::string>>(&str_file_paths),
                     "path list to add to index");
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

            for (auto &str_file_path: str_file_paths) {
                if(str_file_path == "*"){
                    file_paths.clear();
                    fs::recursive_travel(fs::REPOSITORY_ROOT, file_paths);
                    break;
                }else{
                    auto path = boost_fs::path(str_file_path);
                    auto absolute_path = boost_fs::absolute(path);
                    if(!boost_fs::exists(absolute_path)) continue;
                    if(boost_fs::is_directory(absolute_path)){
                        fs::recursive_travel(absolute_path, file_paths);
                    }else{
                        file_paths.push_back(absolute_path);
                    }
                }
            }

            std::sort(file_paths.begin(), file_paths.end());
            file_paths.erase(std::unique(file_paths.begin(), file_paths.end()), file_paths.end());

            auto &global_index = index::Index::instance();
            for (auto &file_path: file_paths) {
                try{
                    auto sha1 = add_one_file(file_path);
                    auto relative_path = boost_fs::relative(file_path, fs::REPOSITORY_ROOT);
                    global_index.remove(relative_path);
                    global_index.add(relative_path, sha1);
                }catch(exceptions::FileException& exception){
                    std::cerr<<exception.what()<<std::endl;
                    continue;
                }
            }
            global_index.save();
        }
    }
}


