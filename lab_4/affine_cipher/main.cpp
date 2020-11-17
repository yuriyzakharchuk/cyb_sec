#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>

constexpr long m = 33;
constexpr std::array<wchar_t, m> alphabeta = {
    L'а', L'б', L'в', L'г', L'ґ',
    L'д', L'е', L'є', L'ж', L'з',
    L'и', L'і', L'ї', L'й', L'к',
    L'л', L'м', L'н', L'о', L'п',
    L'р', L'с', L'т', L'у', L'ф',
    L'х', L'ц', L'ч', L'ш', L'щ',
    L'ь', L'ю', L'я'
};


void
encode(std::basic_ostream<wchar_t>& out_stream, std::wstring* from, size_t a_key, size_t b_key) {
    for(auto ch : *from) {
        auto it { std::find(alphabeta.begin(), alphabeta.end(), ch) };
        if(it != alphabeta.end()) {
            out_stream << static_cast<wchar_t>(alphabeta.at((a_key * (it - alphabeta.begin()) + b_key) % m));
        }
        else {
            out_stream << ch;
        }
    }
}


void
decode(std::basic_ostream<wchar_t>& out_stream, std::wstring* from, size_t a_key, size_t b_key) {
    long a_inverted { 1 };
    while((a_inverted * a_key) % m != 1){
        ++a_inverted;
    }    
    auto tmp = 0;
    if(!std::all_of(from->begin(), from->end(), [b_key](wchar_t x) {
            if(std::find(alphabeta.begin(), alphabeta.end(), x) != alphabeta.end()) {
                return static_cast<long unsigned>(x - alphabeta[0]) >= b_key;
            }
            return true;     
        })) {
        tmp = m;
    }

    for(auto ch : *from) {
        if(std::iswupper(ch)) {
            ch = std::towlower(ch);
        }
        auto it { std::find(alphabeta.begin(), alphabeta.end(), ch) };
        if(it != alphabeta.end()) {
            out_stream << static_cast<wchar_t>(alphabeta.at((a_inverted * ((it - alphabeta.begin()) + tmp - b_key)) % m));
        }
        else {
            out_stream << ch;
        }
    }    
}


std::wstring *
read_file(const char *filepath) {
    std::wifstream t(filepath);
    return new std::wstring (std::istreambuf_iterator<wchar_t>(t), std::istreambuf_iterator<wchar_t>());
}


int
main(int argc, char *argv[]) {
    std::locale::global(std::locale("uk_UA.utf8"));
    std::wcout.setf(std::ios::fixed);
    if(argc != 5) {
        std::wcerr << L"usage: program d text_to_decode a b\n"
                   << "        program e text_to_encode a b"
                   << std::endl;
        std::exit(EXIT_FAILURE);
    }

    long a_key { std::atol(argv[3]) };
    long b_key { std::atol(argv[4]) };

    if(argv[1][0] == 'e') {
        encode(std::wcout, read_file(argv[2]), a_key, b_key);
    }
    else if(argv[1][0] == 'd') {
        decode(std::wcout, read_file(argv[2]), a_key, b_key);
    }
    else {
        std::wcerr << "Error: Undefined flad." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return 0;
}
