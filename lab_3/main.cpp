#include "text_analyzer.h"
#include "textfile_provider.h"


int
main(int argc, char **argv) {
    std::locale::global(std::locale("en_US.utf8"));
    std::wcout.setf(std::ios::fixed);
    if(argc < 2) {
        std::wcerr << L"usage: an path" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    textfile_provider tp(argv[1]);
    for(int i = 2; i < argc; ++i) {
        tp.append(argv[i]);
    }

    text_analyzer ta {tp.get()};
    ta.print_char_frequency(std::wcout, comparator(how_to::descending), 10);
    std::wcout << std::endl;

    ta.print_char_diagram(std::wcout, comparator(how_to::alphabeta), 80);
    std::wcout << std::endl;

    ta.print_bigram_frequency(std::wcout, comparator(how_to::descending), 10);
    std::wcout << std::endl;

    ta.print_bigram_diagram(std::wcout, comparator(how_to::descending), 80, 30);
    std::wcout << std::endl;

    ta.print_trigram_frequency(std::wcout, comparator(how_to::descending), 10);
    std::wcout << std::endl;

    ta.print_trigram_diagram(std::wcout, comparator(how_to::descending), 80, 30);    
    std::wcout << std::endl;
}
