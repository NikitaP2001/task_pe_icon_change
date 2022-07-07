#include <iostream>
#include <string>
#include "main.hpp"
#include "icon.hpp"
#include "pe_file.hpp"


int main(int argc, char *argv[])
{
    char temp_dir[MAX_PATH + 1];
    if (argc == 1) {
        std::cout << "unexpected number of args, provide at least one" << std::endl;
        exit(1);
    }

    // pring image api info
    if (argc >= 2) {

        pe_file pe(argv[1]);

        int WApiNum = pe.print_api_info();
        if (WApiNum == -1)
            std::cerr << "Error reading pe file import info" << std::endl;
        INFO("numbers of api with W: " << WApiNum); // note: print only in debug build

        std::cout << "image entropy: " << pe.entropy() << std::endl;
    }

    // replace icon
    if (argc >= 3) {

        try {

            icon ic(argv[2]);

            if (ic.UpdatePeMainIcon(argv[1]) == 0)
                SUCC("Icon updated");

            std::cout << "icon entropy: " << ic.entropy() << std::endl;

        } catch (std::exception &ex) {
            std::cerr << "Icon update failed: " << ex.what() << std::endl;
        }


    }
}