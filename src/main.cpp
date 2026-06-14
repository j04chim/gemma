#include "gemma.h"
#include <cstdio>

void displayHelp() {
    printf("Gemma tools v%i; gemma [option] file\n", VERSION_NUMBER);
    printf("-h Display this help message.\n");
    printf("-r Regex math against an attribute (-r \"[a-z]\" -i 0)\n");
    printf("-i Return entry of index i\n");
    printf("-m Display metadata");
}

int main(int argc, char** argv) {

    bool r = false;
    std::string r_value = "";
    bool i = false;
    std::string i_value = "";
    bool h = false;
    bool m = false;
    std::string path = "";

    for (int l = 1; l < argc; ++l) {
        if ( argv[l] == std::string("-h") ) {
            h = true;
        } else if ( argv[l] == std::string("-m") ) {
            m = true;
        } else if ( argv[l] == std::string("-i") ) {
            i = true;
        } else if ( argv[l] == std::string("-r") ) {
            r = true;
        } else if ( r && r_value == "") {
            r_value = std::string(argv[l]);
        } else if ( i && i_value == "") {
            i_value = std::string(argv[l]);
        } else {
            path = std::string(argv[l]);
        }
    }

    if (h) {
        displayHelp();
        return 0;
    }

    if (path == "") {
        printf("No file specified.\n");
        displayHelp();
        return 1;
    }

    Gemma* file = new Gemma(path);
    if ( !file->open() ) {
        printf("Cant open file");
        return 1;
    }
    file->buildMap();


    if (r && r_value != "" && i && i_value != "") {

        int nb = 0;
        for (int a: file->getEntriesLike(std::stoi(i_value), r_value)) {
            std::vector<std::string> tmp = file->getEntryAtAdress(a);
            ++nb;
            printf("# %i\n", nb);
            for (std::string s: tmp) {
                printf("    - %s\n", s.c_str());
            }
        }
        printf("Found %i entries!", nb);
        return 0;

    } else if ( i && i_value != "" ) {

        std::vector<std::string> tmp = file->getEntryAtInd(std::stoi(i_value));
        for (std::string s: tmp) {
            printf("    - %s\n", s.c_str());
        }
        return 0;

    } else if ( m ) {

        printf("%s", file->toString().c_str());
        return 0;

    }

    displayHelp();
    return 1;

}