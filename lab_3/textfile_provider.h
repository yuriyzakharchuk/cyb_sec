#ifndef TEXTFILE_PROVIDER
#define TEXTFILE_PROVIDER

#include <string>
#include <fstream>


class textfile_provider {
public:
    explicit
    textfile_provider(const char *);
    
    ~textfile_provider();

    std::wstring *
    get() const;

    void
    append(const char *);

    textfile_provider(const textfile_provider&) = delete;

    textfile_provider &
    operator=(const textfile_provider&) = delete;

private:
    std::wstring *str_;

    std::wstring *
    read_file(const char *);
};


#endif // TEXTFILE_PROVIDER