#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

namespace {
class number
{
    bool m_minus = false;
    std::string_view m_whole_part = "0";
    std::string_view m_fraction = "0";

    std::string_view push_string(int start, std::string_view & str)
    {
        //        int end = start;
        //        while (std::isdigit(str[end])) {
        //            end++;
        //        }
        int end = start + std::distance(str.begin() + start, std::find_if_not(str.begin() + start, str.end(), [](unsigned char i) { return std::isdigit(i); }));
        if (start == end) {
            return "0";
        }
        return str.substr(start, end - start);
    }

    int skip_whitespace(const std::string_view & str)
    {
        return std::distance(str.begin(), std::find_if_not(str.begin(), str.end(), [](char i) { return i == ' '; }));
    }

public:
    number(std::string_view str)
    {
        int i = skip_whitespace(str);
        if (str[i] == '-') {
            m_minus = true;
            i++;
        }
        m_whole_part = push_string(i, str);
        if (str[i] == '.')
            m_fraction = push_string(i + m_whole_part.size() + 1, str);
    }

    friend bool operator==(number & first, number & second)
    {
        return first.m_minus == second.m_minus && first.m_whole_part == second.m_whole_part && first.m_fraction == second.m_fraction;
    }

    friend bool operator<(number & first, number & second)
    {
        if (first.m_minus && !second.m_minus) {
            return true;
        }
        if (!first.m_minus && second.m_minus) {
            return false;
        }
        bool ok = false;
        if (first.m_whole_part.size() != second.m_whole_part.size()) {
            ok = first.m_whole_part.size() > second.m_whole_part.size();
        }
        else if (first.m_whole_part != second.m_whole_part) {
            ok = first.m_whole_part > second.m_whole_part;
        }
        else
            ok = first.m_fraction > second.m_fraction;
        if (!first.m_minus)
            ok = !ok;
        return ok;
    }
};

bool comp_f(std::string & first, std::string & second)
{
    return std::lexicographical_compare(first.begin(), first.end(), second.begin(), second.end(), [](char a, char b) { return toupper(a) < toupper(b); });
}

bool comp_n(std::string & first, std::string & second)
{
    number right = number(first);
    number left = number(second);
    if (right == left) {
        return first < second;
    }
    return right < left;
}
} // namespace

int main(int argc, char ** argv)
{
    std::vector<std::string> strings;
    std::string str;
    std::map<std::string_view, std::function<bool(std::string &, std::string &)>> type_sort{
            {"-f", comp_f},
            {"-n", comp_n},
            {"-nf", comp_n}};
    if (argc != 1 && *argv[argc - 1] != '-') {
        freopen(argv[argc - 1], "r", stdin);
    }
    while (std::getline(std::cin, str)) {
        strings.push_back(std::move(str));
    }
    bool ok = false;
    for (int i = 1; i < argc; ++i)
        if (type_sort.find(argv[i]) != type_sort.end()) {
            sort(strings.begin(), strings.end(), type_sort[argv[i]]);
            ok = true;
        }
    if (!ok)
        sort(strings.begin(), strings.end());
    for (const std::string & i : strings) {
        std::cout << i << '\n';
    }
    return 0;
}