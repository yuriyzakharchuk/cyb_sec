#ifndef TEXT_ANALYZER_HEADER
#define TEXT_ANALYZER_HEADER

#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include <iterator>
#include <fstream>
#include <vector>
#include <functional>

#include "comparator.h"


class text_analyzer
{
public:

    explicit
    text_analyzer(std::wstring*);

    ~text_analyzer();

    text_analyzer(const text_analyzer&) = delete;

    text_analyzer&
    operator=(const text_analyzer&) = delete;

    void
    print_frequency(std::basic_ostream<wchar_t>&,
                    unsigned int columns) const;

    void
    print_char_diagram(std::basic_ostream<wchar_t>&,
                       comparator, unsigned int) const;

    void
    print_bigram_diagram(std::basic_ostream<wchar_t>&,
                         comparator, unsigned int,
                         unsigned int) const;

    void
    print_trigram_diagram(std::basic_ostream<wchar_t>&,
                          comparator, unsigned int,
                          unsigned int) const;

private:
    std::wstring *_text;
    std::vector<std::pair<wchar_t, long double>> *_char_frequency;
    std::vector<std::pair<std::wstring, long double>> *_bigram_frequency;
    std::vector<std::pair<std::wstring, long double>> *_trigram_frequency;

    void
    compute_char_frequency();
    
    void
    compute_bigram_frequency();
    
    void
    compute_trigram_frequency();

    bool
    ignore_char(wchar_t);
};


#endif // TEXT_ANALYZER_HEADER
