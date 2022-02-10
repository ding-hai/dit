//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/10.
//

#ifndef DIT_REFS_H
#define DIT_REFS_H

#include "utils.h"
#include "file_system.h"

namespace dit {
    namespace refs {
        namespace boost_fs = boost::filesystem;

        boost_fs::path get_ref_path();

        std::string read_head();

        bool write_head(const std::string &commit_id);

    }
}


#endif //DIT_REFS_H
