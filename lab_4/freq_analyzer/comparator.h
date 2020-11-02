#ifndef COMPARATOR_HEADER
#define COMPARATOR_HEADER


#include <functional>


enum class how_to {
    ascending,
    descending,
    alphabeta
};


class comparator {
public:
    comparator(how_to method);

    bool
    operator()(std::pair<wchar_t, long double>&,
               std::pair<wchar_t, long double>&);

    bool
    operator()(std::pair<std::wstring, long double>&,
               std::pair<std::wstring, long double>&);

private:
    std::function<bool(std::pair<wchar_t, long double>&,
                       std::pair<wchar_t, long double>&)> _f;

    std::function<bool(std::pair<std::wstring, long double>&,
                       std::pair<std::wstring, long double>&)> _g;

    static bool
    wchar_t_by_ascending(std::pair<wchar_t, long double>&,
                         std::pair<wchar_t, long double>&);
    
    static bool
    wstring_by_ascending(std::pair<std::wstring, long double>&,
                         std::pair<std::wstring, long double>&);

    static bool
    wchar_t_by_descending(std::pair<wchar_t, long double>&,
                          std::pair<wchar_t, long double>&);
    
    static bool
    wstring_by_descending(std::pair<std::wstring, long double>&,
                          std::pair<std::wstring, long double>&);

    static bool
    wchar_t_by_alphabeta(std::pair<wchar_t, long double>&, 
                         std::pair<wchar_t, long double>&);

    static bool
    wstring_by_alphabeta(std::pair<std::wstring, long double>&, 
                         std::pair<std::wstring, long double>&);
};

#endif // COMPARATOR_HEADER