//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//

#include "index.h"

namespace dit {
    namespace index {

        std::string &get_tag(Status status) {
            static std::string tags[]{"same", "rm", "modified", "new file", "last"};
            return tags[status];
        }

        void diff(std::vector<std::pair<std::string, Status>> &result, const std::vector<std::string>& my_lines, const std::vector<std::string>& your_lines){
            //todo myers diff core
            const int n = my_lines.size();
            const int m = your_lines.size();
            const int max = m+n;
            int x=0 , y = 0;
            std::vector<std::map<int,int>> trace(max+1);
            int size_of_trace = 0;

            for(int d=0; d<=max; d++){
                auto &v = trace[d];
                size_of_trace++;
                if(d==0){
                    int t = 0;
                    while(t < n && t < m && my_lines[t] == your_lines[t]) {
                        t++;
                    }

                    v[0] = t;
                    if (t == n && t == m) {
                        goto end_loop;
                    }
                    continue;
                }

                auto &last_v = trace[d-1];

                for(int k = -d; k<=d; k+=2){
                    if (k == -d || (k != d && last_v[k-1] < last_v[k+1])){
                        x = last_v[k+1];
                    }else{
                        x = last_v[k-1] + 1;
                    }

                    y = x - k;

                    while (x < n && y < m && my_lines[x] == your_lines[y]) {
                        x++;
                        y++;
                    }

                    v[k] = x;
                    if (x == n && y == m) {
                        goto end_loop;
                    }
                }

            }

            end_loop: int dummy;
            std::vector<Status> script;

            x = n;
            y = m;

            int k=0, prev_k=0, prev_x=0, prev_y = 0;
            for (int d = size_of_trace - 1; d > 0; d--) {
                k = x- y;
                auto &last_v = trace[d-1];
                if(k==-d || (k!=d && last_v[k-1] < last_v[k+1])){
                    prev_k = k+1;
                }else{
                    prev_k = k-1;
                }

                prev_x = last_v[prev_k];
                prev_y= prev_x - prev_k;

                while(x > prev_x && y> prev_y){
                    script.push_back(SAME);
                    x--;
                    y--;
                }

                if(x == prev_x){
                    script.push_back(ADD);
                }else {
                    script.push_back(DELETE);
                }

                x = prev_x;
                y = prev_y;

            }

            if (trace[0][0] != 0){
                for(int i=0; i<trace[0][0]; i++){
                    script.push_back(SAME);
                }
            }
            std::reverse(script.begin(), script.end());
            int my_idx = 0, your_idx = 0;
            for(auto &status : script){
                switch (status) {
                    case ADD:
                        result.emplace_back(your_lines[your_idx++], ADD);
                        break;
                    case SAME:
                        result.emplace_back(my_lines[my_idx], SAME);
                        my_idx++;
                        your_idx++;
                        break;
                    case DELETE:
                        result.emplace_back(my_lines[my_idx++], DELETE);
                        break;
                }
            }
        }


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

        bool IndexBase::exist(const boost_fs::path &path) const {
            return index_.count(path) > 0;
        }

        const std::string &IndexBase::sha1_of_path(const boost_fs::path &path) const {
            auto it = index_.find(path);
            if (it == index_.end()) {
                return utils::DUMMY_COMMIT_ID;
            }
            return it->second;
        }

        void IndexBase::clear() {
            index_.clear();
        }

        std::unordered_map<boost_fs::path, Status> IndexBase::compare_to(const IndexBase &other) {
            std::set<boost_fs::path> my_keys;
            std::set<boost_fs::path> your_keys;
            std::set<boost_fs::path> union_keys;
            auto keys_of_map = [](const std::unordered_map<boost_fs::path, std::string> &map,
                                  std::set<boost_fs::path> &keys) {
                for (auto &pair: map) {
                    keys.insert(pair.first);
                }
            };
            keys_of_map(index_, my_keys);
            keys_of_map(other.index_, your_keys);

            std::set_union(my_keys.begin(), my_keys.end(), your_keys.begin(), your_keys.end(),
                           std::inserter(union_keys, union_keys.begin()));

            std::unordered_map<boost_fs::path, Status> diffs;
            for (auto &path: union_keys) {
                bool my_exist = this->exist(path);
                bool your_exist = other.exist(path);
                if (my_exist && your_exist) {
                    if (this->sha1_of_path(path) != other.sha1_of_path(path)) {
                        diffs[path] = MODIFY;
                    }else{
                        diffs[path] = SAME;
                    }
                } else if (my_exist) {
                    diffs[path] = ADD;
                } else if (your_exist) {
                    diffs[path] = DELETE;
                }
            }
            return diffs;
        }

        std::vector<std::pair<std::string,Status>> IndexBase::compare_to(const IndexBase &other, const boost_fs::path &file_path) {
            auto exist_in_my_index = this->exist(file_path);
            auto exist_in_other_index = this->exist(file_path);
            objects::BlobObject blob_in_my_index;
            objects::BlobObject blob_in_other_index;
            std::vector<std::pair<std::string,Status>> diffs;
            if(!exist_in_my_index && !exist_in_other_index) {
                //TODO maybe exception case
                return diffs;
            } else if(!exist_in_my_index) {
                //TODO add
                return diffs;
            } else if(!exist_in_other_index) {
                //TODO delete
                return diffs;
            }

            // exist in both index
            auto &sha1_in_my_index = this->sha1_of_path(file_path);
            auto &sha1_in_other_index = other.sha1_of_path(file_path);
            std::vector<std::string> my_lines;
            std::vector<std::string> other_lines;

            blob_in_my_index.read(sha1_in_my_index);
            blob_in_other_index.read(sha1_in_other_index);

            utils::split(my_lines, blob_in_my_index.content(), '\n');
            utils::split(other_lines, blob_in_other_index.content(), '\n');

            diff(diffs, my_lines, other_lines);

            return diffs;
        }

        std::string Index::to_tree_object() {
            boost_fs::path root_path("");
            std::unordered_map<boost_fs::path, objects::TreeObject> path_to_tree_map;
            std::unordered_map<boost_fs::path, std::set<boost_fs::path>> relationships;
            for (auto &pair: index_) {
                auto parent_dir = pair.first.parent_path();
                auto file_name = parent_dir.empty() ? pair.first : boost_fs::relative(pair.first, parent_dir);
                path_to_tree_map[parent_dir].add(100644, objects::BLOB, file_name.generic_string(), pair.second);
                while (!parent_dir.empty()) {
                    relationships[parent_dir.parent_path()].insert(parent_dir);
                    parent_dir = parent_dir.parent_path();
                }
            }

            std::function<const std::string &(const boost_fs::path &)> save_tree;
            save_tree = [&](const boost_fs::path &path) -> const std::string & {
                auto &tree = path_to_tree_map[path];
                for (auto &sub_path: relationships[path]) {
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
            std::vector<boost_fs::path> all_paths;
            fs::recursive_travel(fs::REPOSITORY_ROOT, all_paths);
            std::string file_content;
            for (auto &path: all_paths) {
                fs::file_read(path, file_content);
                std::string sha1 = utils::DUMMY_COMMIT_ID;
                if (boost_fs::is_regular_file(path)) {
                    objects::BlobObject blob(file_content);
                    auto blob_content = blob.to_string();
                    sha1 = utils::sha1digit(blob_content);
                }
                add(boost_fs::relative(path, fs::REPOSITORY_ROOT), sha1);
            }
        }

        void CommitIndex::init_with_commit_object(const objects::CommitObject &commit) {
            objects::TreeObject tree_object;
            tree_object.read(commit.get_root_tree());
            tree_object.expand(index_);
        }

        CommitIndex::CommitIndex(const objects::CommitObject &commit) {
            init_with_commit_object(commit);
        }

        CommitIndex::CommitIndex(const std::string &commit_id) {
            objects::CommitObject commit;
            commit.read(commit_id);
            init_with_commit_object(commit);
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
