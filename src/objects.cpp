//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "objects.h"
#include "exceptions.h"
namespace dit {
    namespace objects {
        Object* BlobObject::from_string(const std::string &file_content) {
            auto index = file_content.find('\n');
            if (index == std::string::npos)
                throw dit::exceptions::MalformedException("blob file is malformed");
            auto header = file_content.substr(0, index);
            this->content_ = file_content.substr(index+1);
            index = header.find(' ');
            auto type = header.substr(0, index);
            auto length = header.substr(index + 1);
            if(type != "blob")
                throw dit::exceptions::MalformedException("type doesn't matched");
            if(std::stoi(length) != this->content_.length())
                throw dit::exceptions::MalformedException("length doesn't matched");
            this->size_ = this->content_.length();
            this->type_ = ObjectType::BLOB;
            return this;
        }

        std::string BlobObject::to_string() {
            std::ostringstream  oss;
            oss<<"blob "<<this->size_<<'\n'<<this->content_;
            return oss.str();
        }

        Object* TreeObject::from_string(const std::string &file_content) {
            return this;
        }

        Object *CommitObject::from_string(const std::string &file_content) {
            return this;
        }
    }
}
