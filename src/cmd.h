//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//

#ifndef DIT_CMD_H
#define DIT_CMD_H
#include <iostream>
#include <unordered_map>
#include <boost/program_options.hpp>
#include "utils.h"
#include "objects.h"
#include "exceptions.h"
#include "index.h"
#include "refs.h"
namespace dit{
    namespace cmd{

        void cmd_add(const std::vector<std::string> &args);
        const std::string &add_one_file(const boost::filesystem::path& file_path);
        void cmd_init(const std::vector<std::string> &args);
        void cmd_commit(const std::vector<std::string> &args);
        void cmd_log(const std::vector<std::string> &args);
        void cmd_reset(const std::vector<std::string> &args);
        bool reset(const std::string &reset_id);
        void cmd_status(const std::vector<std::string> &args);
        bool rm_one_file(const boost::filesystem::path &file_path, index::CommitIndex &commit_index,
                         index::WorkingIndex &working_index, bool force) ;
        void cmd_rm(const std::vector<std::string> &args);
    }

}


#endif //DIT_CMD_H
