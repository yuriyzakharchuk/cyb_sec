#include <iostream>
#include <fstream>
#include <algorithm>


long
gcd(long a, long b) {
    while (a != b) {
        if (a > b) {
            long tmp = a;
            a = b;
            b = tmp;
        }
        b = b - a;
    }
    return a;
}

void
encode(std::basic_ostream<wchar_t>& out_stream, std::wstring* from) {
    int min_index { *std::min_element(from->begin(), from->end()) };
    int m { 26 };
    int a_key { 1 };
    int b_key { m - 1 };

    for(int i = m - 2; i > 1; --i) {
        if(gcd(m, i) == 1) {
            a_key = i;
            break;
        }
    }
    out_stream << "A = " << a_key << ", B = " << b_key << std::endl;
    //E(x) = (ax - b) mod m
    for(auto ch : *from) {
        out_stream << static_cast<wchar_t>(((a_key * (ch - min_index) + b_key) % m) + min_index);
    }
    out_stream << std::endl;
}
 


void
decode(std::basic_ostream<wchar_t>& out_stream, std::wstring* from) {
    
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
    if(argc != 3) {
        std::wcerr << L"usage: program d text_to_decode\n"
                   << "        program e text_to_encode"
                   << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if(argv[1][0] == 'e') {
        encode(std::wcout, read_file(argv[2]));
    }
    else if(argv[1][0] == 'd') {
        decode(std::wcout, read_file(argv[2]));
    }
    else {
        std::wcerr << "Error: Undefined flad." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return 0;
}
