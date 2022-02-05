//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#ifndef DIT_OBJECTS_H
#define DIT_OBJECTS_H

#include <cstdio>
#include <string>
#include "utils.h"
#include "file_system.h"

namespace dit {
    namespace objects {

        enum ObjectType {
            BLOB = 1,
            TREE = 2,
            COMMIT = 3
        };

        class Object {
        protected:
            ObjectType type_;
            size_t size_;
            std::string content_;
            fs::ObjectWriter object_writer_;
            fs::ObjectReader object_reader_;
        public:
            Object() = default;

            Object(ObjectType type, const std::string &content) : type_(type), size_(content.length()),
                                                                  content_(content) {}

            ObjectType type() { return type_; }

            size_t size() { return size_; }

            std::string &content() { return content_; }

            virtual Object *from_string(const std::string &file_content) = 0;

            virtual std::string to_string() = 0;

            std::string write() {
                auto content = to_string();
                return object_writer_.write(content);
            }

            Object* read(std::string &sha1) {
                auto file_content = object_reader_.read(sha1);
                this->from_string(file_content);
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
            Object *from_string(const std::string &file_content) override;
        };

        class CommitObject : public Object {

            Object *from_string(const std::string &file_content) override;
        };
    }
}
#endif //DIT_OBJECTS_H
