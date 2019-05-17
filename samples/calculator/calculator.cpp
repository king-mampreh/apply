// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#include <xt/apply.h>

#include <iostream>
#include <map>

template <typename F, typename G>
auto operator | (F&& f,  G&& g) {
    return [=] (auto... args) {
        return f(args...) || g(args...);
    };
}

const auto pred = std::not_fn(::isascii) | ::iscntrl | ::isblank;

const auto filter = [](auto s) {
    return (s.erase(std::remove_if(s.begin(), s.end(), pred), s.end()), std::move(s));
};

const auto to_string = [](const std::any& any) {
    return filter(std::any_cast<std::string>(any));
};

template <> int
xt::convert<int>::operator()(const std::any& any) { return std::stoi(to_string(any)); }

template <> std::string
xt::convert<std::string>::operator()(const std::string& s) { return filter(s); }

std::map<std::string, std::function<int(int, int)> > dispatch_tbl = {
    {"+",   std::plus<>() },
    {"-",   std::minus<>() },
    {"*",   std::multiplies<>() },
    {"/",   std::divides<>() },
    {"%",   std::modulus<>() },
    {"&",   std::bit_and<>() },
    {"|",   std::bit_or<>() },
    {"^",   std::bit_xor<>() },
    {"fst", [](int a,int) { return a; }},
    {"snd", [](int,int a) { return a; }}
};

int main() {
    while(true) {
        std::cout << "> ";
        try {
            xt::apply_i([=](std::string op, int a, int b) {
                std::cout << "Result: " << dispatch_tbl.at(op)(a, b) << std::endl;
            }, std::istream_iterator<std::string>(std::cin));
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    };
    return 0;
}
