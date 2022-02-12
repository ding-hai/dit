//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//

#ifndef DIT_INDEX_H
#define DIT_INDEX_H

#include <unordered_map>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include "objects.h"


namespace dit {
    namespace index {

        namespace boost_fs = boost::filesystem;
        class CommitIndex;

        enum Status{SAME, DELETE, MODIFY, ADD};

        class IndexBase {
        protected:
            std::unordered_map<boost_fs::path, std::string> index_;
        public:

            size_t remove(const boost_fs::path &path);

            void add(const boost_fs::path &path, const std::string &sha1);

            bool exist(const boost_fs::path &path) const;

            const std::string &sha1_of_path(const boost_fs::path &path) const;

            void clear();

            std::unordered_map<boost_fs::path, Status> compare_to(const IndexBase& other);

        };

        class Index : public IndexBase {
            friend class CommitIndex;
        private:
            const boost_fs::path index_path_;

            Index();

            static Index *instance_;
        public:
            Index(const Index &) = delete;

            Index(Index &&) = delete;

            Index &operator=(const Index &) = delete;

            static Index &instance() {
                if (!instance_)
                    instance_ = new Index();
                return *instance_;
            }

            void save();

            std::string to_tree_object();

        };

        class WorkingIndex : public IndexBase {
        public:
            WorkingIndex();
        };

        class CommitIndex : public IndexBase {
        private:
            void init_with_commit_object(const objects::CommitObject &commit);
        public:
            CommitIndex(const objects::CommitObject &commit);
            CommitIndex(const std::string &commit_id);
            void recover_to_working_dir();
            void swap_to(Index &index);
        };
    }
}


#endif //DIT_INDEX_H
