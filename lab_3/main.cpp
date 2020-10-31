#include "text_analyzer.h"
#include "textfile_provider.h"


int
main(int argc, char **argv) {
    std::locale::global(std::locale("en_US.utf8"));
    std::wcout.setf(std::ios::fixed);
    if(argc != 2) {
        std::wcout << L"usage: an path" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    textfile_provider tp(argv[1]);
    text_analyzer ta {tp.get()};
    ta.print_char_diagram(std::wcout, comparator(how_to::alphabeta), 80);
    std::wcout << std::endl;

    ta.print_bigram_diagram(std::wcout, comparator(how_to::descending), 80, 30);    
}
