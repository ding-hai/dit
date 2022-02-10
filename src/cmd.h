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
        extern std::string ROOT_COMMIT_ID;
        namespace boost_fs = boost::filesystem;
        void cmd_add(const std::vector<std::string> &args);
        const std::string &add_one_file(const boost::filesystem::path& file_path);
        bool cmd_init(const std::string &arg);
        void cmd_commit(const std::vector<std::string> &args);
        void cmd_log();
        void cmd_reset(const std::vector<std::string> &args);
        bool reset(const std::string &reset_id);

    }

}


#endif //DIT_CMD_H
