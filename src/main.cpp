#include <iostream>
#include <string.h>
#include "cmd.h"

int help(){
    std::cout<<"help message"<<std::endl;
    return 0;
}

#define handle(command)                         \
    do {                                        \
        if(::strcmp(argv[1], #command) == 0){   \
            dit::cmd::cmd_##command(args);      \
            return 0;                           \
        }                                       \
    }while(0)

int main(int argc, char** argv) {
    if(argc < 2)
        return help();
    std::vector<std::string> args;
    for(int i=2; i<argc; i++){
        std::cout<<argv[i]<<std::endl;
        args.emplace_back(argv[i]);
    }

    handle(init);
    dit::fs::configure_repository_root();
    handle(add);
    handle(rm);
    handle(commit);
    handle(reset);
    handle(log);
    handle(status);

    return 0;
}
