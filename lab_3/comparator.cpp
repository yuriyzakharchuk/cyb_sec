#include "comparator.h"

comparator::comparator(how_to method) {
    switch (method)
    {
        case how_to::alphabeta: {
            _f = wchar_t_by_alphabeta;
            _g = wstring_by_alphabeta;
            break;
        }
        case how_to::ascending: {
            _f = wchar_t_by_ascending;
            _g = wstring_by_ascending;
            break;
        }
        case how_to::descending: {
            _f = wchar_t_by_descending;
            _g = wstring_by_descending;
            break;
        }
    }
}

bool 
comparator::operator()(std::pair<wchar_t, long double> &a,
                       std::pair<wchar_t, long double> &b) {
    return _f(a, b);
}

bool 
comparator::operator()(std::pair<std::wstring, long double> &a,
                       std::pair<std::wstring, long double> &b) {
    return _g(a, b);
}

bool
comparator::wchar_t_by_ascending(std::pair<wchar_t, long double> &a,
                              std::pair<wchar_t, long double> &b) {
    return a.second < b.second;
}


bool
comparator::wstring_by_ascending(std::pair<std::wstring, long double> &a,
                              std::pair<std::wstring, long double> &b) {
    return a.second < b.second;
}

bool
comparator::wchar_t_by_descending(std::pair<wchar_t, long double> &a,
                              std::pair<wchar_t, long double> &b) {
    return a.second > b.second;
}


bool
comparator::wstring_by_descending(std::pair<std::wstring, long double> &a,
                              std::pair<std::wstring, long double> &b) {
    return a.second > b.second;
}

bool
comparator::wchar_t_by_alphabeta(std::pair<wchar_t, long double> &a,
                              std::pair<wchar_t, long double> &b) {
    return a.first < b.first;
}

bool
comparator::wstring_by_alphabeta(std::pair<std::wstring, long double> &a,
                              std::pair<std::wstring, long double> &b) {
    return a.first < b.first;
}
