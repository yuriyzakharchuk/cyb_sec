#include "text_analyzer.h"
#include <algorithm>
#include <functional>


text_analyzer::text_analyzer(std::wstring *text)
    : _text(text),
      _char_frequency (nullptr),
      _bigram_frequency(nullptr),
      _trigram_frequency(nullptr) {
    compute_char_frequency();
    compute_bigram_frequency();
    compute_trigram_frequency();
}


text_analyzer::~text_analyzer() {
    delete _trigram_frequency;
    _trigram_frequency = nullptr;

    delete _bigram_frequency;
    _bigram_frequency = nullptr;
    
    delete _char_frequency;
    _char_frequency = nullptr;
}


void
text_analyzer::print_char_frequency(std::basic_ostream<wchar_t>& os,
                                    comparator sb, uint32_t columns) const {
    if(_char_frequency->empty()) {
        os << "No char elements." << std::endl;
        return;
    }
    std::sort(std::begin(*_char_frequency), std::end(*_char_frequency), sb);
    os << '\n';
    int flag { 0 };
    for (const auto &pair : *_char_frequency) {
        os << pair.first << ": " << pair.second << (((++flag % columns) != 0)? '\t' : '\n');
    }
    os << std::endl;  
}


void
text_analyzer::print_bigram_frequency(std::basic_ostream<wchar_t>& os,
                                      comparator sb, uint32_t columns) const {
    if(_bigram_frequency->empty()) {
        os << "No bigram elements." << std::endl;
        return;
    }
    std::sort(std::begin(*_bigram_frequency), std::end(*_bigram_frequency), sb);
    os << '\n';
    int flag { 0 };
    for (const auto &pair : *_bigram_frequency) {
        os << pair.first << ": " << pair.second << (((++flag % columns) != 0)? '\t' : '\n');
    }
    os << std::endl;  
}


void
text_analyzer::print_trigram_frequency(std::basic_ostream<wchar_t>& os,
                                       comparator sb, uint32_t columns) const {
    if(_trigram_frequency->empty()) {
        os << "No trigram elements." << std::endl;
        return;
    }
    std::sort(std::begin(*_trigram_frequency), std::end(*_trigram_frequency), sb);
    os << '\n';
    int flag { 0 };
    for (const auto &pair : *_trigram_frequency) {
        os << pair.first << ": " << pair.second << (((++flag % columns) != 0)? '\t' : '\n');
    }
    os << std::endl;  
}


void
text_analyzer::print_char_diagram(std::basic_ostream<wchar_t>& os,
                                  comparator sb, unsigned int row_len) const {
    if(_char_frequency->empty()) {
        os << "No char diagram." << std::endl;
        return;
    }
    std::sort(std::begin(*_char_frequency), std::end(*_char_frequency), sb);
    auto max_value { std::max_element(_char_frequency->begin(), _char_frequency->end(),
        comparator(how_to::ascending))
    };
    for (const auto &pair : *_char_frequency) {
        os << ' ' << pair.first << ' ' << pair.second << ' ';
        for(int i = 0; i < row_len * (pair.second / max_value->second); ++i) {
            os << L'▇';
        }
        os << std::endl;
    }
}


void
text_analyzer::print_bigram_diagram(std::basic_ostream<wchar_t>& os,
                                    comparator sb, unsigned int row_len,
                                    unsigned int print_count) const {
    if(_bigram_frequency->empty()) {
        os << "No bigram diagram." << std::endl;
        return;
    }
    std::sort(std::begin(*_bigram_frequency), std::end(*_bigram_frequency), sb);
    auto max_value { std::max_element(_bigram_frequency->begin(), _bigram_frequency->end(),
        comparator(how_to::ascending))
    };
    unsigned int iteration_count = 0;
    for (const auto &pair : *_bigram_frequency) {
        if(iteration_count++ >= print_count) {
            break;
        }
        os << ' ' << pair.first << ' ' << pair.second << ' ';
        for(int i = 0; i < row_len * (pair.second / max_value->second); ++i) {
            os << L'▇';
        }
        os << std::endl;
    }
}


void
text_analyzer::print_trigram_diagram(std::basic_ostream<wchar_t>& os,
                                     comparator sb, unsigned int row_len,
                                     unsigned int print_count) const {
    if(_trigram_frequency->empty()) {
        os << "No trigram diagram." << std::endl;
        return;
    }
    std::sort(std::begin(*_trigram_frequency), std::end(*_trigram_frequency), sb);
    auto max_value { *std::max_element(_trigram_frequency->begin(), 
        _trigram_frequency->end(), comparator(how_to::ascending))
    };
    unsigned int iteration_count = 0;
    for (const auto &pair : *_trigram_frequency) {
        if(iteration_count++ >= print_count) {
            break;
        }
        os << ' ' << pair.first << ' ' << pair.second << ' ';
        for(int i = 0; i < row_len * (pair.second / max_value.second); ++i) {
            os << L'▇';
        }
        os << std::endl;
    }
}


void
text_analyzer::print_bigram_matrix(std::basic_ostream<wchar_t>& os) const {
    std::sort(std::begin(*_char_frequency), std::end(*_char_frequency), comparator(how_to::alphabeta));
    constexpr auto color_levels { 4 };
    const auto step { 
        std::get<1>(*std::max_element(_bigram_frequency->begin(),
            _bigram_frequency->end(), comparator(how_to::ascending))) / color_levels
    };
    
    os << ' ';
    for(auto i : *_char_frequency) {
        os << ' ' << std::get<0>(i);
    }
    os << '\n';
    
    for(auto i : *_char_frequency) {
        os << std::get<0>(i);
        for(auto j : *_char_frequency) {
            auto current_bigram { std::find_if(_bigram_frequency->begin(),
                _bigram_frequency->end(), [&i, &j](auto &a) { 
                    return std::get<0>(a) == std::wstring(1, std::get<0>(i)) + std::get<0>(j);
                })
            };
            if(current_bigram == _bigram_frequency->end()) {
                os << L"  ";
            }
            else if(current_bigram->second < step) {
                os << L"░░";
            }
            else if(current_bigram->second < 2 * step) {
                os << L"▒▒";
            }
            else if(current_bigram->second < 2 * step + step) {
                os << L"▓▓";
            }
            else {
                os << L"██";
            }
        }
        os << '\n';
    }
}


void
text_analyzer::compute_char_frequency() {
    this->_char_frequency = new std::vector<std::pair<wchar_t, long double>>();
    unsigned long long char_count { 0 }; 

    for(const auto x : *this->_text) {
        if(ignore_char(x) && x != L' ') {
            continue;
        }
        auto iterator { std::find_if(_char_frequency->begin(), _char_frequency->end(),
            [&](const std::pair<wchar_t, long double>& val) {
                return val.first == x;
            })
        };
        if(iterator == _char_frequency->end()) {
            _char_frequency->push_back({x, 1.0});
        }
        else {
            ++iterator->second;
        }
        ++char_count;
    }
    for(auto& pair : *_char_frequency) {
        pair.second /= static_cast<long double>(char_count);
    }
}


void
text_analyzer::compute_bigram_frequency() {
    _bigram_frequency = new std::vector<std::pair<std::wstring, long double>>();
    unsigned long long bigram_count { 0 };
    auto prev_end { std::end(*this->_text) - 1 };

    for(auto it = std::begin(*_text); it != prev_end; ++it) {
        if(ignore_char(*it) || ignore_char(*(it + 1))) {
            continue;
        }
        std::wstring current_bigram { *it };
        current_bigram += *(it + 1); 

        auto iterator { std::find_if(_bigram_frequency->begin(), _bigram_frequency->end(),
            [&](const std::pair<std::wstring, long double>& val) {
                return val.first == current_bigram;
            })
        };
        if(iterator == _bigram_frequency->end()) {
            _bigram_frequency->push_back({current_bigram, 1.0});
        }
        else {
            ++iterator->second;
        }
        ++bigram_count;
    }
    for(auto& pair : *_bigram_frequency) {
        pair.second /= static_cast<long double>(bigram_count);
    }
}


void
text_analyzer::compute_trigram_frequency() {
    _trigram_frequency = new std::vector<std::pair<std::wstring, long double>>();
    unsigned long long trigram_count { 0 };
    auto prev2_end { std::end(*this->_text) - 2 };

    for(auto it = std::begin(*_text); it != prev2_end; ++it) {
        if(ignore_char(*it) || ignore_char(*(it + 1)) || ignore_char(*(it + 2))) {
            continue;
        }
        std::wstring current_trigram { *it };
        current_trigram += *(it + 1); 
        current_trigram += *(it + 2);

        auto iterator { std::find_if(_trigram_frequency->begin(), _trigram_frequency->end(),
            [&](const std::pair<std::wstring, long double>& val) {
                return val.first == current_trigram;
            })
        };
        if(iterator == _trigram_frequency->end()) {
            _trigram_frequency->push_back({current_trigram, 1.0});
        }
        else {
            ++iterator->second;
        }
        ++trigram_count;
    }
    for(auto& pair : *_trigram_frequency) {
        pair.second /= static_cast<long double>(trigram_count);
    }
}


inline bool
text_analyzer::ignore_char(wchar_t ch) {
    return !std::isalpha(ch, std::locale("uk_UA.utf8"));
}

