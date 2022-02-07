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

namespace dit {
    namespace objects {

        enum ObjectType {
            UNDEFINED = 0,
            BLOB = 1,
            TREE = 2,
            COMMIT = 3
        };

        class Object {
        protected:
            ObjectType type_ = BLOB;
            size_t size_ = 0;
            utils::CharSequence content_;
            fs::ObjectWriter object_writer_;
            fs::ObjectReader object_reader_;
        public:
            Object() = default;

            Object(ObjectType type, const utils::CharSequence &content) : type_(type), size_(content.length()),
                                                                          content_(content) {}

            ObjectType type() { return type_; }

            size_t size() { return size_; }

            utils::CharSequence &content() { return content_; }

            virtual Object *from_char_sequence(const utils::CharSequence &file_content) = 0;

            virtual utils::CharSequence to_char_sequence() = 0;

            std::string write() {
                auto content = to_char_sequence();
                boost::uuids::detail::sha1::digest_type sha1;
                return object_writer_.write(content, sha1);
            }

            std::string write_with_raw_sha1(boost::uuids::detail::sha1::digest_type &sha1) {
                auto content = to_char_sequence();
                return object_writer_.write(content, sha1);
            }

            Object *read(const std::string &sha1) {
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
        private:
            std::vector<std::string> parents;
            std::string author;
            std::string committer;
            TreeObject tree;

            Object *from_char_sequence(const utils::CharSequence &file_content) override;

            utils::CharSequence to_char_sequence() override;
        };
    }
}
#endif //DIT_OBJECTS_H
