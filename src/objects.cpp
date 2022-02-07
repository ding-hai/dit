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
            if (std::stoi((char *) length.data()) != this->content_.length())
                throw dit::exceptions::MalformedException("length doesn't matched");
            this->size_ = this->content_.length();
            this->type_ = ObjectType::BLOB;
            return this;
        }

        utils::CharSequence BlobObject::to_char_sequence() {
            utils::CharSequence sequence;
            sequence.append("blob ")
                    .append(this->size_)
                    .append('\0')
                    .append(this->content_);
            return std::move(sequence);
        }

        Object *TreeObject::from_char_sequence(const utils::CharSequence &file_content) {
            auto index = file_content.find('\0');
            if (index == utils::CharSequence::npos)
                throw dit::exceptions::MalformedException("blob file is malformed");
            auto header = file_content.sub_sequence(0, index);
            this->content_ = file_content.sub_sequence(index + 1);
            index = header.find(' ');
            if (index == utils::CharSequence::npos)
                throw dit::exceptions::MalformedException("blob file is malformed");
            auto type = header.sub_sequence(0, index);
            auto length = header.sub_sequence(index + 1);
            if (type != "tree")
                throw dit::exceptions::MalformedException("type doesn't matched");
            if (std::stoi((char *) length.data()) != this->content_.length())
                throw dit::exceptions::MalformedException("length doesn't matched");

            auto remain = this->content_;
            while (remain.length() > 0) {
                index = remain.find('\0');
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
            this->size_ = this->content_.length();
            return this;
        }

        utils::CharSequence TreeObject::to_char_sequence() {
            utils::CharSequence sequence;
            utils::CharSequence body;

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

            sequence.append("tree ")
                    .append(body.length())
                    .append('\0')
                    .append(body);
            return sequence;
        }

        void TreeObject::add(int mode, ObjectType type, const std::string &file_path,
                             boost::uuids::detail::sha1::digest_type &sha1) {
            this->items.emplace_back(mode, type, file_path, sha1);
        }

        Object *CommitObject::from_char_sequence(const utils::CharSequence &file_content) {
            return this;
        }

        utils::CharSequence CommitObject::to_char_sequence() {
            utils::CharSequence sequence;
            return sequence;
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

