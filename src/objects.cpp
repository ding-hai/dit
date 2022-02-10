//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "objects.h"
#include "exceptions.h"

namespace dit {
    namespace objects {
        std::string &object_type_to_string(ObjectType type) {
            static std::string map_from_type_to_string[]{"UNDEFINED", "blob", "tree", "commit"};
            return map_from_type_to_string[type];
        }

        ObjectType string_to_object_type(std::string s) {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
            if (s == "blob")
                return BLOB;
            if (s == "tree")
                return TREE;
            if (s == "commit")
                return COMMIT;
            return UNDEFINED;
        }


        Object *BlobObject::from_string(const std::string &file_content) {
            Object::from_string(file_content);
            this->type_ = ObjectType::BLOB;
            return this;
        }

        std::string BlobObject::to_string() {
            return Object::to_string();
        }

        Object *TreeObject::from_string(const std::string &file_content) {
            Object::from_string(file_content);
            items.clear();
            std::vector<std::string> lines;
            std::vector<std::string> line_items;
            utils::split(lines, this->content_, '\n');
            for (auto &line: lines) {
                line_items.clear();
                utils::split(line_items, line, ' ');
                items.emplace_back(std::stoi(line_items[0]), string_to_object_type(line_items[1]), line_items[2],
                                   line_items[3]);
            }
            this->type_ = TREE;
            return this;
        }

        std::string TreeObject::to_string() {
            std::ostringstream oss;
            for (auto &item: items) {
                oss << item.mode
                    << ' '
                    << object_type_to_string(item.type)
                    << ' '
                    << item.file_path
                    << ' '
                    << item.sha1
                    << '\n';
            }
            this->content_ = oss.str();
            return Object::to_string();
        }

        void TreeObject::add(int mode, ObjectType type, const std::string &file_path,
                             const std::string &sha1) {
            this->items.emplace_back(mode, type, file_path, sha1);
        }

        void TreeObject::expand(std::unordered_map<boost::filesystem::path, std::string> &path_to_sha1_map) {
            for (auto &item: items) {
                Object *object;
                if (item.type == TREE)
                    object = new TreeObject();
                else
                    object = new BlobObject();
                object->read(item.sha1);
                if (item.type == TREE)
                    dynamic_cast<TreeObject *>(object)->expand(path_to_sha1_map);
                auto &&file_path = boost::filesystem::path(item.file_path);
                path_to_sha1_map[file_path] = sha1;
                index.emplace(item.file_path, object);
            }
        }

        Object *CommitObject::from_string(const std::string &file_content) {
            Object::from_string(file_content);
            auto body = this->content_;

            auto limiter = body.find("\n\n");
            commit_msg = body.substr(limiter + 2); // need to be trimmed
            body = body.substr(0, limiter);

            std::vector<std::string> lines;
            utils::split(lines, body, '\n');

            std::vector<std::string> line_items;
            for (auto &line: lines) {
                line_items.clear();
                utils::split(line_items, line, ' ');
                auto line_type = line_items[0];
                if (line_type == "tree") {
                    root_tree_sha1 = line_items[1];
                } else if (line_type == "parent") {
                    parents.push_back(line_items[1]);
                } else if (line_type == "author") {
                    author.name_ = line_items[1];
                    author.email_ = line_items[2];
                    timestamp = line_items[3];
                } else if (line_type == "commit") {
                    committer.name_ = line_items[1];
                    committer.email_ = line_items[2];
                    timestamp = line_items[3];
                }
            }

            return this;
        }

        std::string CommitObject::to_string() {
            std::ostringstream oss;

            // add tree
            oss << "tree " << root_tree_sha1 << '\n';

            // add parents
            for (auto &parent_commit: parents) {
                oss << "parent " << parent_commit << '\n';
            }

            // add author and committer
            oss << "author " << author.name_ << ' ' << author.email_ << ' ' << timestamp << '\n';
            oss << "committer " << committer.name_ << ' ' << committer.email_ << ' ' << timestamp << '\n';
            oss << '\n' << commit_msg;

            this->content_ = oss.str();
            return Object::to_string();
        }

        void CommitObject::log(std::ostringstream &oss) {
            std::time_t time;
            std::istringstream iss(timestamp);
            iss >> time;

            oss << "commit\t"<<sha1 << '\n'
                << "Author:\t" << author.name_ <<" <"<< author.email_ << ">\n"
                << "Date:\t" << std::asctime(std::localtime(&time))
                << '\n'
                << '\t' << commit_msg
                << "\n\n";
        }

        const std::vector<std::string> &CommitObject::get_parents() const {
            return parents;
        }

        void CommitObject::add_parent(const std::string &parent_sha1) {
            parents.push_back(parent_sha1);
        }

        const std::string &CommitObject::get_root_tree() const {
            return root_tree_sha1;
        }

        void CommitObject::set_root_tree(const std::string &tree_sha1) {
            root_tree_sha1 = tree_sha1;
        }

        const CommitObject::User &CommitObject::get_author() const {
            return author;
        }

        void CommitObject::set_author(const CommitObject::User &author) {
            CommitObject::author = author;
        }

        const CommitObject::User &CommitObject::get_committer() const {
            return committer;
        }

        void CommitObject::set_committer(const CommitObject::User &committer) {
            CommitObject::committer = committer;
        }

        const std::string &CommitObject::get_timestamp() const {
            return timestamp;
        }

        void CommitObject::set_timestamp(const std::string &timestamp) {
            CommitObject::timestamp = timestamp;
        }

        const std::string &CommitObject::get_commit_msg() const {
            return commit_msg;
        }

        void CommitObject::set_commit_msg(const std::string &commit_msg) {
            CommitObject::commit_msg = commit_msg;
        }

        TreeObject::Item::Item(int mode, ObjectType type, const std::string &file_path,
                               const std::string &sha1) {
            this->mode = mode;
            this->type = type;
            this->file_path = file_path;
            this->sha1 = sha1;
        }
    }
}

