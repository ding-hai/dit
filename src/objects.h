//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_OBJECTS_H
#define DIT_OBJECTS_H

#include <cstdio>
#include <string>
#include <algorithm>
#include <map>
#include <unordered_map>
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

        std::string &object_type_to_string(ObjectType type) ;

        ObjectType string_to_object_type(std::string s);

        class Object {
        protected:
            ObjectType type_ = BLOB;
            std::string content_;
            fs::ObjectWriter object_writer_;
            fs::ObjectReader object_reader_;
        public:
            std::string sha1;
        public:
            Object() = default;

            Object(ObjectType object_type) : type_(object_type) {}

            Object(ObjectType type, const std::string &content) : type_(type), content_(content) {}

            ObjectType type() { return type_; }

            std::string &content() { return content_; }

            virtual Object *from_string(const std::string &file_content) {
                auto index = file_content.find('\n');
                if (index == std::string::npos)
                    throw dit::exceptions::MalformedException("file is malformed");
                auto header = file_content.substr(0, index);
                this->content_ = file_content.substr(index + 1);
                index = header.find(' ');
                if (index == std::string::npos)
                    throw dit::exceptions::MalformedException("file is malformed");
                auto type = header.substr(0, index);
                auto length = header.substr(index + 1);
                if (type != "tree" && type != "blob" && type != "commit")
                    throw dit::exceptions::MalformedException("file is malformed");
                if (std::stoi((char *) length.data()) != this->content_.length())
                    throw dit::exceptions::MalformedException("length doesn't matched");
                return this;
            };

            virtual std::string to_string() {
                auto type = object_type_to_string(this->type_);
                std::ostringstream oss;
                oss << type << ' ' << this->content_.length() << '\n' << this->content_;
                return std::move(oss.str());
            };

            virtual const std::string& write() {
                auto content = to_string();
                sha1 = object_writer_.write(content);
                return sha1;
            }

            virtual Object *read(const std::string &sha1) {
                auto file_content = object_reader_.read(sha1);
                this->from_string(file_content);
                this->sha1 = sha1;
                return this;
            }
        };

        class BlobObject : public Object {
        public:
            BlobObject() = default;

            BlobObject(const std::string &content) : Object(BLOB, content) {}

            Object *from_string(const std::string &file_content) override;

            std::string to_string() override;
        };


        class TreeObject : public Object {
            struct Item {
                Item(int mode, ObjectType type, const std::string &file_path, const std::string &sha1);

                int mode;
                ObjectType type = ObjectType::UNDEFINED;
                std::string file_path;
                std::string sha1;
            };

        public:
            std::vector<Item> items;
            std::map<std::string, Object *> index;
        public:
            TreeObject() : Object(TREE) {};
            ~TreeObject() {
                for(auto &pair : index){
                    auto *p = pair.second;
                    delete p;
                }
            };

            Object *from_string(const std::string &file_content) override;

            std::string to_string() override;

            void
            add(int mode, ObjectType type, const std::string &file_path,const  std::string &sha1);

            void expand(std::unordered_map<boost::filesystem::path, std::string> &path_to_sha1_map);
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

            Object *from_string(const std::string &file_content) override;

            std::string to_string() override;
            void log(std::ostringstream &oss);


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
