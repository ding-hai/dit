//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//

#include "objects.h"
#include "exceptions.h"

namespace dit {
    namespace objects {
        Object *BlobObject::from_char_sequence(const utils::CharSequence &file_content) {
            auto index = file_content.find('\0');
            if (index == utils::CharSequence::npos)
                throw dit::exceptions::MalformedException("blob file is malformed");
            auto header = file_content.sub_sequence(0, index);
            this->content_ = file_content.sub_sequence(index + 1);
            index = header.find(' ');
            auto type = header.sub_sequence(0, index);
            auto length = header.sub_sequence(index + 1);
            if (type != "blob")
                throw dit::exceptions::MalformedException("type doesn't matched");
            if (std::stoi(length.data()) != this->content_.length())
                throw dit::exceptions::MalformedException("length doesn't matched");
            this->size_ = this->content_.length();
            this->type_ = ObjectType::BLOB;
            return this;
        }

        utils::CharSequence BlobObject::to_char_sequence() {
            utils::CharSequence sequence;
            sequence.append("blob ")
                    .append(this->size_)
                    .append('\n')
                    .append(this->content_);
            return std::move(sequence);
        }

        Object *TreeObject::from_char_sequence(const utils::CharSequence &file_content) {
            return this;
        }

        utils::CharSequence TreeObject::to_char_sequence() {
            utils::CharSequence sequence;
            return sequence;
        }

        Object *CommitObject::from_char_sequence(const  utils::CharSequence &file_content) {
            return this;
        }

        utils::CharSequence CommitObject::to_char_sequence() {
            utils::CharSequence sequence;
            return sequence;
        }
    }
}
