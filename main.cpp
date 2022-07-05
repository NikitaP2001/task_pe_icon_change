#include <iostream>
#include <string>
#include "file.hpp"

int main(int argc, char *argv[])
{

    if (argc == 1) {
        std::cout << "unexpected number of args, provide at least one" << std::endl;
        exit(1);
    }
    if (argc == 2) {
        pe_file(std::string(argv[1]));
        std::cout << "show info only" << std::endl;
    }
    if (argc >= 3)
        std::cout << "replace ico" << std::endl;

}