//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//
#include <gtest/gtest.h>
#include <objects.h>
#include <unordered_map>
#include <queue>

TEST(Util, CharSequence) {
    const char *c_str = "1234";
    dit::utils::CharSequence sequence(c_str);
    EXPECT_TRUE(sequence.length() == 4);
    EXPECT_TRUE(sequence.capacity() == 8);

    dit::utils::CharSequence sequence1 = sequence;
    EXPECT_TRUE(sequence1 == sequence);

    auto ptr = sequence1.data();
    dit::utils::CharSequence sequence2 = std::move(sequence1);
    EXPECT_TRUE(sequence1.data() == nullptr);
    EXPECT_TRUE(sequence1.size() == 0);
    EXPECT_TRUE(sequence1.capacity() == 0);
    EXPECT_TRUE(sequence2.data() == ptr);

    char buffer[] = {'1', '2', '3', '4', '\0', '5'};
    sequence.append('\0');
    sequence.append(size_t(5));
    EXPECT_TRUE(sequence.length() == sizeof buffer);
    for (int i = 0; i < sequence.length(); i++) {
        EXPECT_TRUE(buffer[i] == sequence.data()[i]);
    }
}

TEST(SHA1, sha1digit_to_char_seq) {
    dit::utils::CharSequence input("hello world");
    dit::utils::CharSequence output;
    boost::uuids::detail::sha1::digest_type sha1;
    dit::utils::sha1digit(input, sha1);
    dit::utils::sha1digit_to_char_seq(output, sha1);
    const uint8_t *data = output.data();
    uint8_t expected[20]{0x2a, 0xae, 0x6c, 0x35, 0xc9, 0x4f, 0xcf, 0xb4, 0x15, 0xdb, 0xe9, 0x5f, 0x40, 0x8b, 0x9c, 0xe9,
                         0x1e, 0xe8, 0x46, 0xed};
    EXPECT_EQ(output.length(), 20);
    for (int i = 0; i < output.length(); ++i) {
        EXPECT_TRUE(data[i] == expected[i]);
    }
}

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
        boost::uuids::detail::sha1::digest_type sha1;
        dit::utils::sha1digit(file_content, sha1);
        file_to_sha1_map.emplace(file_path, dit::utils::sha1digit_to_string(sha1));
        tree_object_write.add(100644, dit::objects::BLOB, file_path, sha1);
    }

    auto sha1_str = tree_object_write.write();
    tree_object_read.read(sha1_str);
    EXPECT_TRUE(file_to_sha1_map.size() == tree_object_read.items.size());
    for (auto &item: tree_object_read.items) {
        EXPECT_TRUE(file_to_sha1_map[item.file_path] == dit::utils::sha1digit_to_string(item.sha1));
    }
}


TEST(Object, TreeExpand) {
    dit::objects::TreeObject tree_object_write;
    dit::objects::TreeObject sub_tree;
    dit::objects::TreeObject sub_sub_tree;
    std::string base_file_path("file_");
    std::string base_folder("folder/");
    boost::uuids::detail::sha1::digest_type sha1;
    typedef std::pair<std::string, dit::objects::ObjectType> Sha1AndTypePair;
    std::unordered_map<std::string, Sha1AndTypePair> file_to_sha1_map;
    for (int i = 0; i < 100; i++) {
        dit::objects::BlobObject blob(dit::utils::CharSequence("hello world ").append(size_t(i)));
        std::string file_path;
        auto sha1_str = blob.write_with_raw_sha1(sha1);
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
        file_to_sha1_map.emplace(file_path, Sha1AndTypePair(sha1_str, dit::objects::BLOB));
    }

    auto sub_sub_tree_sha1_str = sub_sub_tree.write_with_raw_sha1(sha1);
    sub_tree.add(400000, dit::objects::TREE, base_folder + base_folder, sha1);
    auto sub_tree_sha1_str = sub_tree.write_with_raw_sha1(sha1);
    tree_object_write.add(400000, dit::objects::TREE, base_folder, sha1);
    auto root_tree_sha1_string = tree_object_write.write();

    file_to_sha1_map.emplace(base_folder, Sha1AndTypePair(sub_tree_sha1_str, dit::objects::TREE));
    file_to_sha1_map.emplace(base_folder + base_folder, Sha1AndTypePair(sub_sub_tree_sha1_str, dit::objects::TREE));

    dit::objects::TreeObject tree_object_read;
    tree_object_read.read(root_tree_sha1_string);
    tree_object_read.expand();
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
                dit::utils::CharSequence sequence("tree ");
                sequence.append((size_t) sub_tree->content().length())
                        .append('\0')
                        .append(sub_tree->content());
                EXPECT_EQ(pair.first, dit::utils::sha1digit(sequence));

                validate_result(*sub_tree);
            } else {
                auto *blob = dynamic_cast<dit::objects::BlobObject *>(obj);
                dit::utils::CharSequence sequence("blob ");
                sequence.append((size_t) blob->content().length())
                        .append('\0')
                        .append(blob->content());
                EXPECT_EQ(pair.first, dit::utils::sha1digit(sequence));
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
        dit::objects::BlobObject blob(dit::utils::CharSequence("hello world ").append(size_t(i)));
        auto sha1_str = blob.write_with_raw_sha1(sha1);
        tree_object_write.add(100644, dit::objects::BLOB, base_file_path + std::to_string(i), sha1);
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


