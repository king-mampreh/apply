## Welcome!
The C++ Apply library is a King Mamphreh project to extend functionality of function std::apply to enable to work with std::vector, std::list, std::set, ... 

## Synopsis

```C++
#include <xt/apply.h>

#include <iostream>
#include <sstream>

const auto sum3 = [](int a, int b, int c) { return a + b + c; };

int main() {
    do {
        std::cout << "Enter three numbers: ";
        try {
            std::ostringstream os;
            os << "Result: " << xt::apply_i(sum3, std::istream_iterator<int>(std::cin));
               
            std::cout << os.str() << std::endl;
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    } while(true);
    
    return 0;
}
```

```C++
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

const auto to_string = [](std::any& any) {
    return filter(std::any_cast<std::string>(any));
};

template <> int
xt::convert<int>(std::any any) {
    return std::stoi(to_string(any));
}

template <> std::string 
xt::convert<std::string>(std::any any) {
    return to_string(any);
}

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
            xt::apply_i([=](std::string& op, int a, int b) {
                std::cout << "Result: " << dispatch_tbl.at(op)(a, b) << std::endl;
            }, std::istream_iterator<std::string>(std::cin));
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    };
    return 0;
}
```

```C++

#include <xt/apply.h>

#include <functional>
#include <any>
#include <vector>
#include <list>
#include <set>
#include <iostream>

using namespace std;

int sum3(int a, int b, int c) {
    return a + b + c;
}

int no_args() {
    return rand();
}

int sum(int a, int b) {
    return a + b;
}

float sum_i_f(int a, float b) {
    return a + b;
}

class Prod {
public:
    int operator()(int a, int b) {
        return a * b;
    }
};

int main() {
    
    vector<int> v{9, 12, 2};
    list<int> l{ 7, 5 };
    set<int> s = { 7, 5 };
    list<any> ll = { 1, 1.5f };
    list<int> l1 = {19};
    
    cout << xt::apply(&sum3, v) << endl; // Output: 23
    
    cout << xt::apply(no_args, {}) << endl;
    
    cout << xt::apply(&sum, l) << endl; // 12
    
    cout << xt::apply(sum, l) << endl; // 12
    
    cout << xt::apply(&sum, s) << endl; // 12
    
    cout << xt::apply(&sum_i_f, ll) << endl; // 2.5
    
    cout << xt::apply([](int a, int b) { return a + b; }, l) << endl; // 12
    
    cout << xt::apply(Prod(), {4, 5}) << endl;  // 20

    
    using namespace placeholders;
    
    function<int(int)> sum4 = bind(sum, 4, _1);
    
    cout << xt::apply(sum4, l1) << endl; // 23
    
    auto sum5 = [](int a) { return sum(5, a); };
    
    cout << xt::apply(sum5, l1) << endl; // 24
    
    cout << xt::apply(&sum, {1, 3}) << endl; // 4
    
    return 0;
}
```

## License

Copyright (c) King Mampreh. All rights reserved.

Licensed under the [MIT License](./LICENSE).
