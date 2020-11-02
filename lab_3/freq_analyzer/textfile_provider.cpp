#include "textfile_provider.h"

textfile_provider::textfile_provider(const char *filepath) 
    : str_(textfile_provider::read_file(filepath)) {
}

textfile_provider::~textfile_provider() {
    if(str_ != nullptr) {
        delete str_;
        str_ = nullptr;
    }        
}


std::wstring *
textfile_provider::get() const {
    return str_;
}


std::wstring *
textfile_provider::read_file(const char *filepath) {
    std::wifstream t(filepath);
    return new std::wstring (std::istreambuf_iterator<wchar_t>(t), std::istreambuf_iterator<wchar_t>());
}


void
textfile_provider::append(const char *filepath) {
    auto suffix_str { this->read_file(filepath) };
    str_->operator+=(*suffix_str);
    delete suffix_str;
}
