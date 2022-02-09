//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_OBJECTS_H
#define DIT_OBJECTS_H

#include <cstdio>
#include <string>
#include <map>
#include "utils.h"
#include "file_system.h"
#include "exceptions.h"

namespace dit {
    namespace objects {

        enum ObjectType {
            UNDEFINED = 0,
            BLOB = 1,
            TREE = 2,
            COMMIT = 3
        };

        static const char *ObjectTypes[]{"UNDEFINED", "blob", "tree", "commit"};

        class Object {
        protected:
            ObjectType type_ = BLOB;
            size_t size_ = 0;
            utils::CharSequence content_;
            fs::ObjectWriter object_writer_;
            fs::ObjectReader object_reader_;
        public:
            Object() = default;

            Object(ObjectType object_type) : type_(object_type) {}

            Object(ObjectType type, const utils::CharSequence &content) : type_(type), content_(content) {}

            ObjectType type() { return type_; }

            utils::CharSequence &content() { return content_; }

            virtual Object *from_char_sequence(const utils::CharSequence &file_content) {
                auto index = file_content.find('\0');
                if (index == utils::CharSequence::npos)
                    throw dit::exceptions::MalformedException("file is malformed");
                auto header = file_content.sub_sequence(0, index);
                this->content_ = file_content.sub_sequence(index + 1);
                index = header.find(' ');
                if (index == utils::CharSequence::npos)
                    throw dit::exceptions::MalformedException("file is malformed");
                auto type = header.sub_sequence(0, index);
                auto length = header.sub_sequence(index + 1);
                if (type != "tree" && type != "blob" && type != "commit")
                    throw dit::exceptions::MalformedException("file is malformed");
                if (std::stoi((char *) length.data()) != this->content_.length())
                    throw dit::exceptions::MalformedException("length doesn't matched");
                return this;
            };

            virtual utils::CharSequence to_char_sequence() {
                auto type = ObjectTypes[this->type_];
                utils::CharSequence sequence;
                sequence.append(type)
                        .append(' ')
                        .append(this->content_.length())
                        .append('\0')
                        .append(this->content_);
                return std::move(sequence);
            };

            virtual std::string write() {
                auto content = to_char_sequence();
                boost::uuids::detail::sha1::digest_type sha1;
                return object_writer_.write(content, sha1);
            }

            virtual std::string write_with_raw_sha1(boost::uuids::detail::sha1::digest_type &sha1) {
                auto content = to_char_sequence();
                return object_writer_.write(content, sha1);
            }

            virtual Object *read(const std::string &sha1) {
                auto file_content = object_reader_.read(sha1);
                this->from_char_sequence(file_content);
                return this;
            }
        };

        class BlobObject : public Object {
        public:
            BlobObject() = default;

            BlobObject(const utils::CharSequence &content) : Object(BLOB, content) {}

            Object *from_char_sequence(const utils::CharSequence &file_content) override;

            utils::CharSequence to_char_sequence() override;
        };


        class TreeObject : public Object {
            struct Item {
                Item(int mode, ObjectType type, const std::string &file_path,
                     boost::uuids::detail::sha1::digest_type &sha1);

                int mode;
                ObjectType type = ObjectType::UNDEFINED;
                std::string file_path;
                boost::uuids::detail::sha1::digest_type sha1;
            };

        public:
            std::vector<Item> items;
            std::map<std::string, Object *> index;
        public:
            TreeObject() : Object(TREE) {};

            Object *from_char_sequence(const utils::CharSequence &file_content) override;

            utils::CharSequence to_char_sequence() override;

            void
            add(int mode, ObjectType type, const std::string &file_path, boost::uuids::detail::sha1::digest_type &sha1);

            void expand() {
                for (auto &item: items) {
                    Object *object;
                    if (item.type == TREE)
                        object = new TreeObject();
                    else
                        object = new BlobObject();
                    object->read(utils::sha1digit_to_string(item.sha1));
                    if (item.type == TREE)
                        dynamic_cast<TreeObject *>(object)->expand();

                    index.emplace(item.file_path, object);
                }
            }
        };


        class CommitObject : public Object {
        public:
            struct User {
                std::string name_;
                std::string email_;

                User() = default;

                User(const std::string &name, const std::string &email) : name_(name), email_(email) {};
            };

        private:
            std::vector<std::string> parents;
            std::string root_tree_sha1;
            // author and committer is same as each other currently
            User author;
            User committer;
            std::string timestamp;
            std::string commit_msg;

        public:
            CommitObject() : Object(COMMIT) {};

            Object *from_char_sequence(const utils::CharSequence &file_content) override;

            utils::CharSequence to_char_sequence() override;

            void add_parent(const std::string &parent_sha1);

            const std::vector<std::string> &get_parents() const;

            const std::string &get_root_tree() const;

            void set_root_tree(const std::string &tree_sha1);

            const User &get_author() const;

            void set_author(const User &author);

            const User &get_committer() const;

            void set_committer(const User &committer);

            const std::string &get_timestamp() const;

            void set_timestamp(const std::string &timestamp);

            const std::string &get_commit_msg() const;

            void set_commit_msg(const std::string &commit_msg);

        };
    }
}
#endif //DIT_OBJECTS_H
