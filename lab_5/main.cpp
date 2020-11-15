// Base64 encoding/decoding from https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <cstring>

extern "C" {
#include <openssl/evp.h>
}

constexpr size_t KEY_SIZEOF { 16 };
constexpr size_t IV_SIZEOF { 16 };
constexpr size_t UPDATE_BUFFER { 4 };

static constexpr char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static constexpr char reverse_table[128] = {
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
        64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};


class file_reader {
public:
    explicit file_reader(const char *filepath)
            : str_(read_file(filepath)) {
    }

    [[nodiscard]] const std::string&
    get() const {
        return str_;
    }

private:
    std::string str_;

    std::string
    read_file(const std::string &filepath) {
        std::ifstream t(filepath);
        return std::string (std::istreambuf_iterator<char>(t),
                        std::istreambuf_iterator<char>());
    }
};


std::string
base64_encode(const std::string plaintext) {
    const std::size_t binlen = plaintext.size();
    std::string ret_val((((binlen + 2) / 3) * 4), '=');
    std::size_t out_pos       { 0 };
    unsigned int bits_collected       { 0 };
    unsigned int accumulator { 0 };
    const auto bin_end { plaintext.cend() };

    for (auto i = plaintext.cbegin(); i != bin_end; ++i) {
        accumulator = (accumulator << 8u) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            ret_val[out_pos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) {
        assert(bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        ret_val[out_pos++] = b64_table[accumulator & 0x3fu];
    }
    return ret_val;
}

std::string
base64_decode(const std::string &ciphertext) {
    std::string retval;
    const auto last = ciphertext.cend();
    unsigned int bits_collected = 0;
    unsigned int accumulator = 0;

    for (auto i = ciphertext.cbegin(); i != last; ++i) {
        const int c = *i;
        if (std::isspace(c) || c == '=') {
            continue;
        }
        if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
            std::cerr << "Error: illegal character." << std::endl;
            std::exit(-1);
        }
        accumulator = (accumulator << 6u) | reverse_table[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            retval += static_cast<char>((accumulator >> bits_collected) & 0xffu);
        }
    }
    return retval;
}


std::string
aes_128_ctr_encrypt(const std::string &plaintext, const std::string &key, const std::string &iv) {
    EVP_CIPHER_CTX *cipher { nullptr };
    cipher = EVP_CIPHER_CTX_new();
    if(!cipher) {
        std::cerr << "Error: EVP_CIPHER_CTX_new()" << std::endl;
        std::exit(-1);
    }
    if(EVP_EncryptInit_ex(cipher, EVP_aes_128_ctr(), nullptr,
                          reinterpret_cast<const unsigned char *>(key.c_str()),
                          reinterpret_cast<const unsigned char *>(iv.c_str())) != 1) {
        std::cerr << "Error: EVP_CIPHER_CTX_new()" << std::endl;
        std::exit(-1);
    }

    auto current_len     { 0 };
    auto cipher_shift    { 0 };
    auto plaintext_shift { 0 };
    auto current_bufsize { UPDATE_BUFFER };
    auto plaintext_ptr   { reinterpret_cast<const unsigned char *>(plaintext.c_str()) };
    auto ciphertext      {
        static_cast<unsigned char *>(std::calloc(plaintext.length() + 1, sizeof(unsigned  char)))
    };

    EVP_CIPHER_CTX_set_padding(cipher, 0);
    while(plaintext_shift <=  plaintext.length() - current_bufsize) {
        if(1 != EVP_EncryptUpdate(cipher,
                             ciphertext + cipher_shift,
                             &current_len,
                             plaintext_ptr + plaintext_shift ,
                             current_bufsize)) {
            std::cerr << "Error: EVP_EncryptUpdate()" << std::endl;
            std::exit(-1);
        }
        cipher_shift += current_len;
        plaintext_shift += current_bufsize;
    }
    current_bufsize = plaintext.length() - plaintext_shift;
    if(1 != EVP_EncryptUpdate(cipher,
                         ciphertext + cipher_shift,
                         &current_len,
                         plaintext_ptr + plaintext_shift,
                         current_bufsize)) {
        std::cerr << "Error: EVP_EncryptUpdate()" << std::endl;
        std::exit(-1);
    }

    if(EVP_EncryptFinal_ex(cipher, ciphertext, &current_len) != 1) {
        std::cerr << "Error: EVP_EncryptFinal_ex()" << std::endl;
        std::exit(-1);
    }

    EVP_CIPHER_CTX_free(cipher);
    std::string ret_val { reinterpret_cast<const char *>(ciphertext) };
    std::free(ciphertext);
    return ret_val;
}


std::string
aes_128_ctr_decrypt(const std::string &ciphertext, const std::string &key, const std::string &iv) {
    EVP_CIPHER_CTX *cipher { nullptr };
    cipher = EVP_CIPHER_CTX_new();
    if(!cipher) {
        std::cerr << "Error: EVP_CIPHER_CTX_new()" << std::endl;
        std::exit(-1);
    }

    if(EVP_DecryptInit_ex(cipher, EVP_aes_128_ctr(), nullptr,
                          reinterpret_cast<const unsigned char *>(key.c_str()),
                          reinterpret_cast<const unsigned char *>(iv.c_str())) != 1) {
        std::cerr << "Error: EVP_CIPHER_CTX_new()" << std::endl;
        std::exit(-1);
    }

    auto current_len    { 0 };
    auto ciphertext_ptr { reinterpret_cast<const unsigned char *>(ciphertext.c_str()) };
    auto plaintext      {
            static_cast<unsigned char *>(std::calloc(ciphertext.length() + 1, sizeof(unsigned  char)))
    };

    if(EVP_DecryptUpdate(cipher, plaintext, &current_len, ciphertext_ptr, ciphertext.length()) != 1) {
        std::cerr << "Error: EVP_DecryptUpdate()" << std::endl;
        std::exit(-1);
    }

    if(EVP_DecryptFinal_ex(cipher, plaintext + current_len, &current_len) != 1) {
        std::cerr << "Error: EVP_DecryptFinal_ex()" << std::endl;
        std::exit(-1);
    }

    EVP_CIPHER_CTX_free(cipher);
    std::string ret_val { reinterpret_cast<const char *>(plaintext) };
    std::free(plaintext);
    return ret_val;
}


int
main(int argc, char *argv[]) {
    const std::string invalid_usage {
            "Usage: program d|e file key init_vector"
    };
    if(argc != 5) {
        std::cerr << invalid_usage << std::endl;
        std::exit(-1);
    }

    auto key { std::string(argv[3]) };
    auto iv  { std::string(argv[4]) };

    if(key.length() != KEY_SIZEOF || iv.length() != IV_SIZEOF) {
        std::cerr << "Error: incorrect key or iv." << std::endl;
        std::exit(-1);
    }
    file_reader fr { argv[2] };
    if(fr.get().empty()) {
        std::cerr << "Error: file empty or not found." << std::endl;
        std::exit(-1);
    }
    if(argv[1][0] == 'e') {
        auto encrypted {
            aes_128_ctr_encrypt(fr.get(), key, iv)
        };
        std::cout << base64_encode(encrypted) << std::endl;
    }
    else if(argv[1][0] == 'd') {
        auto decrypted {
            aes_128_ctr_decrypt(base64_decode(fr.get()), key, iv)
        };
        std::cout << decrypted << std::endl;
    }
    else {
        std::cerr << "Flag error! " << invalid_usage << std::endl;
        std::exit(-1);
    }
    return 0;
}
