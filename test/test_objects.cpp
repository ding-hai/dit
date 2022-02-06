//
// Created by ding-hai (https://ding-hai.github.io/) on 2022/2/5.
//
#include <gtest/gtest.h>
#include <objects.h>

TEST(Util, CharSequence){
    const char *c_str = "1234";
    dit::utils::CharSequence sequence(c_str);
    EXPECT_TRUE(sequence.length() == 4);
    EXPECT_TRUE(sequence.capacity() == 8);
}

TEST(Object, Blob) {
    std::string content("hello\ndit\nfirst test of blob");
    dit::objects::BlobObject *blob_object_write = new dit::objects::BlobObject(content);
    auto sha1 = blob_object_write->write();

    dit::objects::BlobObject *blob_object_read = new dit::objects::BlobObject();
    blob_object_read->read(sha1);
    EXPECT_TRUE(blob_object_read->content() ==  content);
}
