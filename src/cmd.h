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
namespace dit{
    namespace cmd{
        namespace boost_fs = boost::filesystem;
        void cmd_add(const std::vector<std::string> &args);
        const std::string &add_one_file(const boost::filesystem::path& file_path);
        bool cmd_init(const std::string &arg);

    }

}


#endif //DIT_CMD_H
