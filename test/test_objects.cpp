//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//
#include <gtest/gtest.h>
#include <objects.h>
#include <unordered_map>
#include <queue>
#include <cmd.h>

TEST(Object, Blob) {
    std::string content("hello\ndit\nfirst test of blob");
    dit::objects::BlobObject blob_object_write(content);
    auto sha1 = blob_object_write.write();

    dit::objects::BlobObject blob_object_read;
    blob_object_read.read(sha1);
    EXPECT_TRUE(blob_object_read.content() == content);
}

TEST(Object, TreeReadAndWrite) {
    dit::objects::TreeObject tree_object_write;
    dit::objects::TreeObject tree_object_read;
    std::string base_file_path("file_item_");
    std::string base_file_content("content_");

    std::unordered_map<std::string, std::string> file_to_sha1_map;
    for (int i = 0; i < 1024; ++i) {
        auto str = std::to_string(i);
        auto file_path = base_file_path + str;
        auto file_content = base_file_content + str;
        auto sha1 = dit::utils::sha1digit(file_content);
        file_to_sha1_map.emplace(file_path, sha1);
        tree_object_write.add(100644, dit::objects::BLOB, file_path, sha1);
    }

    auto sha1_str = tree_object_write.write();
    tree_object_read.read(sha1_str);
    EXPECT_TRUE(file_to_sha1_map.size() == tree_object_read.items.size());
    for (auto &item: tree_object_read.items) {
        EXPECT_TRUE(file_to_sha1_map[item.file_path] == item.sha1);
    }
}


TEST(Object, TreeExpand) {
    dit::objects::TreeObject tree_object_write;
    dit::objects::TreeObject sub_tree;
    dit::objects::TreeObject sub_sub_tree;
    std::string base_file_path("file_");
    std::string base_folder("folder/");
    typedef std::pair<std::string, dit::objects::ObjectType> Sha1AndTypePair;
    std::unordered_map<std::string, Sha1AndTypePair> file_to_sha1_map;
    for (int i = 0; i < 100; i++) {
        dit::objects::BlobObject blob(std::string("hello world ").append(std::to_string(i)));
        std::string file_path;
        auto sha1 = blob.write();
        if (i % 2 == 0) {
            file_path = base_file_path + std::to_string(i);
            tree_object_write.add(100644, dit::objects::BLOB, file_path, sha1);
        } else if (i % 3 == 0) {
            file_path = base_folder + std::to_string(i);
            sub_tree.add(100644, dit::objects::BLOB, file_path, sha1);
        } else {
            file_path = base_folder + base_folder + std::to_string(i);
            sub_sub_tree.add(100644, dit::objects::BLOB, file_path, sha1);
        }
        file_to_sha1_map.emplace(file_path, Sha1AndTypePair(sha1, dit::objects::BLOB));
    }

    auto sub_sub_tree_sha1_str = sub_sub_tree.write();
    sub_tree.add(400000, dit::objects::TREE, base_folder + base_folder, sub_sub_tree_sha1_str);
    auto sub_tree_sha1_str = sub_tree.write();
    tree_object_write.add(400000, dit::objects::TREE, base_folder, sub_tree_sha1_str);
    auto root_tree_sha1_string = tree_object_write.write();

    file_to_sha1_map.emplace(base_folder, Sha1AndTypePair(sub_tree_sha1_str, dit::objects::TREE));
    file_to_sha1_map.emplace(base_folder + base_folder, Sha1AndTypePair(sub_sub_tree_sha1_str, dit::objects::TREE));

    dit::objects::TreeObject tree_object_read;
    tree_object_read.read(root_tree_sha1_string);
    std::unordered_map<boost::filesystem::path, std::string> path_to_sha1_map;
    tree_object_read.expand(path_to_sha1_map);

    EXPECT_TRUE(tree_object_read.items.size() == 51);
    EXPECT_TRUE(tree_object_read.index.size() == 51);
    std::function<void(dit::objects::TreeObject &)> validate_result;
    validate_result = [&](dit::objects::TreeObject &tree) {
        for (auto it = tree.index.begin(); it != tree.index.end(); ++it) {
            auto &file_path = it->first;
            std::cout << file_path << std::endl;
            auto &pair = file_to_sha1_map[file_path];
            auto &obj = it->second;
            EXPECT_EQ(obj->type(), pair.second);

            if (obj->type() == dit::objects::TREE) {
                auto *sub_tree = dynamic_cast<dit::objects::TreeObject *>(obj);
                std::ostringstream sequence;
                sequence << "tree "
                         << sub_tree->content().length()
                         << '\n'
                         << sub_tree->content();
                EXPECT_EQ(pair.first, dit::utils::sha1digit(sequence.str()));

                validate_result(*sub_tree);
            } else {
                auto *blob = dynamic_cast<dit::objects::BlobObject *>(obj);
                std::ostringstream sequence;
                sequence << "blob "
                         << blob->content().length()
                         << '\n'
                         << blob->content();
                EXPECT_EQ(pair.first, dit::utils::sha1digit(sequence.str()));
            }

        }
    };

    validate_result(tree_object_read);

    std::cout << std::endl;

}


TEST(Object, Commit) {
    dit::objects::TreeObject tree_object_write;
    std::string base_file_path("file_");
    boost::uuids::detail::sha1::digest_type sha1;
    for (int i = 0; i < 100; i++) {
        dit::objects::BlobObject blob(std::string("hello world ").append(std::to_string(i)));
        auto sha1_str = blob.write();
        tree_object_write.add(100644, dit::objects::BLOB, base_file_path + std::to_string(i), sha1_str);
    }

    auto tree_sha1_str = tree_object_write.write();
    dit::objects::CommitObject commit_object_write;
    auto name = "ding-hai";
    auto email = "dhairoot@gmail.com";
    dit::objects::CommitObject::User author(name, email);
    dit::objects::CommitObject::User committer(name, email);
    auto commit_msg = "hello commit";
    auto time = std::time(nullptr);
    auto timestamp_str = std::to_string(time);
    auto parent_sha1 = "0000000000000000000000000000000000000000";
    commit_object_write.add_parent(parent_sha1);
    commit_object_write.set_author(author);
    commit_object_write.set_committer(committer);
    commit_object_write.set_commit_msg(commit_msg);
    commit_object_write.set_timestamp(timestamp_str);
    commit_object_write.set_root_tree(tree_sha1_str);
    auto commit_sha1 = commit_object_write.write();

    dit::objects::CommitObject commit_object_read;
    commit_object_read.read(commit_sha1);
    auto &root_tree_sha1_read = commit_object_read.get_root_tree();
    EXPECT_EQ(root_tree_sha1_read, tree_sha1_str);
    auto &parents = commit_object_read.get_parents();
    EXPECT_EQ(parents.size(), 1);
    EXPECT_TRUE(parents[0] == parent_sha1);

    auto timestamp_read = commit_object_read.get_timestamp();
    EXPECT_EQ(timestamp_read, timestamp_str);
    auto author_read = commit_object_read.get_author();
    auto committer_read = commit_object_write.get_committer();
    EXPECT_EQ(author_read.name_, author.name_);
    EXPECT_EQ(author_read.email_, author.email_);
    EXPECT_EQ(committer_read.name_, committer.name_);
    EXPECT_EQ(committer_read.email_, committer_read.email_);

    auto commit_msg_read = commit_object_read.get_commit_msg();
    auto commit_msg_str = std::string(commit_msg);
    EXPECT_EQ(commit_msg_read, commit_msg_str);

}

TEST(CMD, add_one_file) {
    boost::filesystem::remove_all(".dit");
    const char *file_path = "cmd.txt";
    std::string content;
    dit::fs::file_read(file_path, content);
    auto sha1 = dit::cmd::add_one_file(file_path);
    dit::objects::BlobObject blob;
    blob.read(sha1);
    EXPECT_EQ(blob.content(), content);
}

// todo make it more automated
TEST(CMD, cmd_add) {
    boost::filesystem::remove_all(".dit");
    dit::cmd::cmd_init("");
    dit::fs::configure_repository_root();
    std::vector<std::string> args{"*"};
    dit::cmd::cmd_add(args);
}

TEST(Index, to_tree_object){
    boost::filesystem::remove_all(".dit");
    dit::cmd::cmd_init("");
    dit::fs::configure_repository_root();
    std::vector<std::string> args{"*"};
    dit::cmd::cmd_add(args);
    auto &index = dit::index::Index::instance();
    auto root_tree_sha1 = index.to_tree_object();
    dit::objects::CommitObject commit;
    commit.set_root_tree(root_tree_sha1);
    dit::objects::CommitObject::User author("dinghai", "dhairoot@126.com");
    dit::objects::CommitObject::User committer("dinghai", "dhairoot@126.com");
    commit.set_committer(committer);
    commit.set_author(author);
    commit.set_timestamp(std::to_string(std::time(nullptr)));
    commit.set_commit_msg("hello first commit");
    auto commit_sha1 = commit.write();
    std::cout<<commit_sha1<<std::endl;
}

TEST(CMD, cmd_commit){
    boost::filesystem::remove_all(".dit");
    dit::cmd::cmd_init("");
    dit::fs::configure_repository_root();
    std::vector<std::string> args_of_cmd_add1{"cmd.txt"};
    std::vector<std::string> args_of_cmd_commit1{"--m", "hello first commit"};
    dit::cmd::cmd_add(args_of_cmd_add1);
    dit::cmd::cmd_commit(args_of_cmd_commit1);

    std::vector<std::string> args_of_cmd_add2{"src/1.txt"};
    std::vector<std::string> args_of_cmd_commit2{"--m", "hello second commit"};
    dit::cmd::cmd_add(args_of_cmd_add2);
    dit::cmd::cmd_commit(args_of_cmd_commit2);

    std::vector<std::string> args_of_cmd_add3{"src/sub/"};
    std::vector<std::string> args_of_cmd_commit3{"--m", "hello 3rd commit"};
    dit::cmd::cmd_add(args_of_cmd_add3);
    dit::cmd::cmd_commit(args_of_cmd_commit3);
}