#include <iostream>
#include <string>
#include "main.hpp"
#include "file.hpp"

int main(int argc, char *argv[])
{

    if (argc == 1) {
        std::cout << "unexpected number of args, provide at least one" << std::endl;
        exit(1);
    }
    if (argc == 2) {

        pe_file pe(argv[1]);

        int WApiNum = pe.print_api_info();
        if (WApiNum == -1)
            std::cerr << "Error reading pe file import info" << std::endl;
        INFO("numbers of api with W: " << WApiNum); // note: only in debug build
    }
    if (argc >= 3)
        std::cout << "replace ico" << std::endl;

}