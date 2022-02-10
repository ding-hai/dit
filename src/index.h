//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/9.
//

#ifndef DIT_INDEX_H
#define DIT_INDEX_H

#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include "objects.h"


namespace dit {
    namespace index {

        namespace boost_fs = boost::filesystem;

        class IndexBase {
        protected:
            std::unordered_map<boost_fs::path, std::string> index_;
        public:

            size_t remove(const boost_fs::path &path);

            void add(const boost_fs::path &path, const std::string &sha1);

            bool exist(const boost_fs::path &path);

            const std::string &sha1_of_path(const boost_fs::path &path);

            void clear();
        };

        class Index : public IndexBase {
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
        };

        class WorkingIndex : public IndexBase {
        public:
            WorkingIndex();
        };

        class CommitIndex : public IndexBase {
        public:
            CommitIndex(const std::string &sha1);
        };
    }
}


#endif //DIT_INDEX_H
