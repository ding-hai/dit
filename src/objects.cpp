//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "objects.h"
#include "exceptions.h"

namespace dit {
    namespace objects {
        Object *BlobObject::from_char_sequence(const utils::CharSequence &file_content) {
            Object::from_char_sequence(file_content);
            this->type_ = ObjectType::BLOB;
            return this;
        }

        utils::CharSequence BlobObject::to_char_sequence() {
            return Object::to_char_sequence();
        }

        Object *TreeObject::from_char_sequence(const utils::CharSequence &file_content) {
            Object::from_char_sequence(file_content);
            auto remain = this->content_;
            while (remain.length() > 0) {
                auto index = remain.find('\0');
                if (index + 20 > remain.length())
                    break;

                auto line_header = remain.sub_sequence(0, index);
                auto sha1_seq = remain.sub_sequence(index + 1, index + 21);
                remain = remain.sub_sequence(index + 21);

                boost::uuids::detail::sha1::digest_type sha1;
                utils::char_seq_to_sha1digit(sha1_seq, sha1);

                index = line_header.find(' ');
                auto mode_sequence = line_header.sub_sequence(0, index);
                int mode = std::stoi((char *) mode_sequence.data());

                auto tmp_sequence = line_header.sub_sequence(index + 1);
                index = tmp_sequence.find(' ');
                auto type_sequence = tmp_sequence.sub_sequence(0, index);
                auto type = type_sequence == "tree" ? TREE : BLOB;

                auto file_path_sequence = tmp_sequence.sub_sequence(index + 1);
                std::string file_path((char *) file_path_sequence.data());
                items.emplace_back(mode, type, file_path, sha1);
            }
            this->type_ = TREE;
            return this;
        }

        utils::CharSequence TreeObject::to_char_sequence() {
            auto &body = this->content_;
            body.clear();

            for (auto &item: items) {
                utils::CharSequence sha1_seq;
                utils::sha1digit_to_char_seq(sha1_seq, item.sha1);

                body.append(size_t(item.mode))
                        .append(' ')
                        .append(item.type == TREE ? "tree" : "blob")
                        .append(' ')
                        .append(item.file_path)
                        .append('\0')
                        .append(sha1_seq);
            }

            return Object::to_char_sequence();
        }

        void TreeObject::add(int mode, ObjectType type, const std::string &file_path,
                             boost::uuids::detail::sha1::digest_type &sha1) {
            this->items.emplace_back(mode, type, file_path, sha1);
        }

        Object *CommitObject::from_char_sequence(const utils::CharSequence &file_content) {
            Object::from_char_sequence(file_content);
            auto &body = this->content_;

            // the input string does include commit message
            auto str = body.str();
            auto limiter = str.find("\n\n");
            commit_msg = str.substr(limiter + 2); // need to be trimmed
            str = str.substr(0, limiter);

            std::vector<std::string> lines;
            utils::split(lines, str, '\n');
            auto parse_user = [](std::string &remain, User &user) -> std::string {
                std::vector<std::string> items;
                utils::split(items, remain, ' ');
                user.name_ = items[0];
                user.email_ = items[1];
                auto time = items[2];
                return time;
            };
            for (auto &line: lines) {
                auto index = line.find(' ');
                auto line_type = line.substr(0, index);
                auto remain = line.substr(index + 1);
                if (line_type == "tree") {
                    root_tree_sha1 = remain;
                } else if (line_type == "parent") {
                    parents.push_back(remain);
                } else if (line_type == "author") {
                    timestamp = parse_user(remain, author);
                } else if (line_type == "commit") {
                    timestamp = parse_user(remain, committer);
                }
            }

            return this;
        }

        utils::CharSequence CommitObject::to_char_sequence() {
            auto &body = this->content_;
            body.clear();

            // add tree
            body.append("tree ").append(root_tree_sha1).append('\n');

            // add parents
            for (auto &parent_commit: parents) {
                body.append("parent ").append(parent_commit).append('\n');
            }

            auto append_user = [&body](const char *type, const User &user, const std::string time) {
                body.append(type).append(' ')
                        .append(user.name_).append(' ')
                        .append(user.email_).append(' ')
                        .append(time).append('\n');
            };
            // add author and committer
            append_user("author", author, timestamp);
            append_user("committer", committer, timestamp);
            body.append('\n').append(commit_msg);


            return Object::to_char_sequence();
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
                               boost::uuids::detail::sha1::digest_type &sha1) {
            this->mode = mode;
            this->type = type;
            this->file_path = file_path;
            for (int i = 0; i < 5; i++)
                this->sha1[i] = sha1[i];
        }
    }
}

