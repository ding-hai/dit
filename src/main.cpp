#include <iostream>
#include "objects.h"
#include "exceptions.h"

int main() {
    std::string s("blob 7\n123\n321");
    try {
        dit::objects::BlobObject blob(s);
        std::cout << blob.size() << blob.type() << blob.content() << std::endl;
    } catch (dit::exceptions::MalformedException &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
