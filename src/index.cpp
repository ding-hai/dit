//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//

#include "index.h"

namespace dit {
    namespace index {

        size_t IndexBase::remove(const boost_fs::path &path) {
            if (path.empty()) return 0;
            std::vector<boost_fs::path> to_be_removed_paths;
            const auto &target_path = path.generic_string();
            for (const auto &pair: index_) {
                const auto item_path = pair.first.generic_string();
                if (item_path == target_path ||
                    (item_path.find(target_path) == 0 && item_path.length() > target_path.length() &&
                     item_path.at(target_path.length()) == '/')) {
                    to_be_removed_paths.push_back(pair.first);
                }
            }

            for (auto &to_be_removed_path: to_be_removed_paths) {
                index_.erase(to_be_removed_path);
            }
            return to_be_removed_paths.size();
        }

        void IndexBase::add(const boost_fs::path &path, const std::string &sha1) {
            index_[path] = sha1;
        }

        bool IndexBase::exist(const boost_fs::path &path) {
            return index_.count(path) > 0;
        }

        const std::string &IndexBase::sha1_of_path(const boost_fs::path &path) {
            return index_[path];
        }

        void IndexBase::clear() {
            index_.clear();
        }

        Index *Index::instance_ = nullptr;

        Index::Index():index_path_( dit::fs::REPOSITORY_ROOT / dit::fs::REPOSITORY_INTERNAL_PATH/dit::fs::INDEX) {
            std::string file_content;
            auto success = dit::fs::file_read(index_path_, file_content);
            if(!success) return;

            std::istringstream iss(file_content);
            int n = 0;
            iss >> n;
            index_.reserve(n);
            std::string path;
            std::string sha1;
            for (int i = 0; i < n; i++) {
                iss >> sha1 >> path;
                add(path, sha1);
            }
        }

        void Index::save() {
            std::ostringstream oss;
            oss << index_.size() << '\n';
            for (auto &pair: index_) {
                oss << pair.second << ' ' << pair.first.generic_string() << '\n';
            }
            dit::fs::file_write(index_path_, oss.str());
        }

        WorkingIndex::WorkingIndex() {

        }

        CommitIndex::CommitIndex(const std::string &sha1) {
            objects::CommitObject commit_object;
            commit_object.read(sha1);
            objects::TreeObject tree_object;
            tree_object.read(commit_object.get_root_tree());
            tree_object.expand(index_);
        }
    }
}
