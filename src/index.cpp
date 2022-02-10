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

        std::string Index::to_tree_object() {
            boost_fs::path root_path("");
            std::unordered_map<boost_fs::path, objects::TreeObject> path_to_tree_map;
            std::unordered_map<boost_fs::path, std::set<boost_fs::path>> relationships;
            for (auto &pair: index_) {
                auto &&parent_dir = pair.first.parent_path();
                auto &&file_name = parent_dir.empty() ? pair.first : boost_fs::relative(pair.first, parent_dir);
                path_to_tree_map[parent_dir].add(100644, objects::BLOB, file_name.generic_string(), pair.second);
                while (!parent_dir.empty()) {
                    relationships[parent_dir.parent_path()].insert(parent_dir);
                    parent_dir = parent_dir.parent_path();
                }
            }

            std::function<const std::string&(const boost_fs::path& )> save_tree;
            save_tree = [&](const boost_fs::path& path) -> const std::string&{
                auto &tree = path_to_tree_map[path];
                for(auto &sub_path: relationships[path]){
                    auto sha1 = save_tree(sub_path);
                    tree.add(400000, objects::TREE, sub_path.generic_string(), sha1);
                }
                return tree.write();
            };

            auto sha1 = save_tree(root_path);

            return sha1;
        }

        Index *Index::instance_ = nullptr;

        Index::Index() : index_path_(dit::fs::REPOSITORY_ROOT / dit::fs::REPOSITORY_INTERNAL_PATH / dit::fs::INDEX) {
            std::string file_content;
            auto success = dit::fs::file_read(index_path_, file_content);
            if (!success) return;

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

        CommitIndex::CommitIndex(const objects::CommitObject &commit) {
            objects::TreeObject tree_object;
            tree_object.read(commit.get_root_tree());
            tree_object.expand(index_);
        }

        void CommitIndex::recover_to_working_dir() {
            for (auto &pair: index_) {
                objects::BlobObject blob;
                blob.read(pair.second);
                auto parent_path = pair.first.parent_path();
                parent_path = boost_fs::absolute(parent_path, fs::REPOSITORY_ROOT);
                if (!boost_fs::exists(parent_path))
                    boost_fs::create_directories(parent_path);
                fs::file_write(pair.first, blob.content());
            }
        }

        void CommitIndex::swap_to(Index &index) {
            index.index_ = index_;
        }


    }
}
