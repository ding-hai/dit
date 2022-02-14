//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//
#include "cmd.h"

namespace dit {
    namespace cmd {

        void cmd_status(const std::vector<std::string> &args) {
            typedef std::vector<std::string> GroupedResultType[index::Status::LAST];
            auto group_by_status = [](
                    const std::unordered_map<boost_fs::path, index::Status> &diffs,
                    GroupedResultType &grouped_diffs) {
                for (auto &pair: diffs) {
                    grouped_diffs[pair.second].push_back(pair.first.generic_string());
                }
            };
            auto log_diffs = [](const GroupedResultType &grouped_diffs) {
                for (int status = index::Status::SAME + 1; status < index::Status::LAST; status++) {
                    if (!grouped_diffs[status].empty()) {
                        std::string tag = index::get_tag(static_cast<index::Status>(status));
                        for (auto &file_path: grouped_diffs[status]) {
                            std::cout << '\t' << tag << ":\t" << file_path << std::endl;
                        }
                    }
                }

            };
            GroupedResultType grouped_diffs;
            auto &global_index = index::Index::instance();

            // compare index and commit
            auto head_commit_id = refs::read_head();
            index::CommitIndex *commit_index = nullptr;
            if (head_commit_id != utils::DUMMY_COMMIT_ID)
                commit_index = new index::CommitIndex(head_commit_id);
            else
                commit_index = new index::CommitIndex;

            auto diffs_between_commit_and_index = global_index.compare_to(*commit_index);
            delete commit_index;
            commit_index = nullptr;
            group_by_status(diffs_between_commit_and_index, grouped_diffs);
            std::cout << "Changes to be committed:" << std::endl;
            log_diffs(grouped_diffs);

            for (int i = 0; i < index::Status::LAST; i++)
                grouped_diffs[i].clear();

            // compare between working directory and index
            index::WorkingIndex working_index;
            auto diffs_between_index_and_working_dir = working_index.compare_to(global_index);
            group_by_status(diffs_between_index_and_working_dir, grouped_diffs);
            std::cout << "Changes not staged for commit:" << std::endl;
            log_diffs(grouped_diffs);

        }
    }
}


