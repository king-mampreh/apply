// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <tuple>
#include <functional>

namespace xt {

template <bool callable, typename R, typename C, typename... Args>
struct method_details {
    using class_t = typename std::conditional<callable, void, C>::type;

    using return_t = R;

    enum {
        arity = sizeof...(Args),
        xarity = sizeof...(Args) + !callable
    };

    using args_t = std::tuple<Args...>;
    using xargs_t = typename std::conditional<callable, args_t, std::tuple<std::add_pointer_t<class_t>, Args...>>::type;

    template <size_t i>
    struct arg {
        using type = typename std::tuple_element<i, args_t>::type;
    };

    template <size_t i>
    struct xarg {
        using type = typename std::tuple_element<i, xargs_t>::type;
    };

    template <size_t i>
    using arg_t = typename arg<i>::type;

    template <size_t i>
    using xarg_t = typename xarg<i>::type;
};

template <typename R, typename... Args>
struct function_details : method_details<true, R, void, Args...> {};

template <typename T, bool callable = false>
struct details;

template <bool c, typename R, typename C, typename... Args>
struct details<R (C::*)(Args...), c> : method_details<c, R, C, Args...> {};

template <bool c, typename R, typename C, typename... Args>
struct details<R (C::*)(Args...) const, c> : method_details<c, R, C, Args...> {};

template <bool c, typename R, typename C, typename... Args>
struct details<R (C::*)(Args..., ...), c> : method_details<c, R, C, Args...> {};

template <bool c, typename R, typename C, typename... Args>
struct details<R (C::*)(Args..., ...) const, c> : method_details<c, R, C, Args...> {};

template <typename R, typename... Args>
struct details<R (Args...)> : function_details<R, Args...> {};

template <typename R, typename... Args>
struct details<R (Args... , ...)> : function_details<R, Args...> {};

template <typename T>
struct details<const T> : details<T> {};

template <typename T>
struct details<volatile T> : details<T> {};

template <typename T>
struct details<T&> : details<T> {};

template <typename T>
struct details<T*> : details<T> {};

class ___test;

static_assert(std::is_same_v<details<void (___test::*)()>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::*)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* const)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* const)()>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* volatile)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* volatile)()>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::*)() const>::return_t, void>);

static_assert(std::is_same_v<details<void (___test::*&)()>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::*&)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* const &)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::* volatile &)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::*&)() const>::return_t, void>);
static_assert(std::is_same_v<details<void (___test::*&)()>::return_t, void>);

static_assert(std::is_same_v<details<void (*)()>::return_t, void>);
static_assert(std::is_same_v<details<void (* const)()>::return_t, void>);
static_assert(std::is_same_v<details<void (* volatile)()>::return_t, void>);

static_assert(std::is_same_v<details<void (*&)()>::return_t, void>);
static_assert(std::is_same_v<details<void (* const &)()>::return_t, void>);
static_assert(std::is_same_v<details<void (* volatile &)()>::return_t, void>);

template <typename T, bool callable>
struct details : details<decltype(&T::operator()), true> {};

class ___test { ___test() { auto l = []{};
    static_assert(std::is_same_v<details<decltype(l)>::return_t, void>);
    static_assert(std::is_same_v<details<decltype(&l)>::return_t, void>);
    static_assert(std::is_same_v<details<decltype(l)&>::return_t, void>);
    static_assert(std::is_same_v<details<const decltype(l)&>::return_t, void>);
    static_assert(std::is_same_v<details<const decltype(l)*>::return_t, void>);
}; };

/**
 * Returns the number of the function arguments.
 */
template <typename T>
struct arity {
    enum { value = details<T>::arity };
};

template <typename T>
struct xarity {
    enum { value = details<T>::xarity };
};

static_assert(arity<void (int, int)>::value == 2);
static_assert(arity<void (int, int, int)>::value == 3);
static_assert(arity<void (int, int, int, ...)>::value == 3);

static_assert(arity<void (___test::*)()>::value == 0);
static_assert(arity<void (___test::*)(int)>::value == 1);
static_assert(arity<void (___test::*)(int, int)>::value == 2);
static_assert(arity<void (___test::*)(int, int, int)>::value == 3);

static_assert(xarity<void (___test::*)()>::value == 1);
static_assert(xarity<void (___test::*)(int)>::value == 2);
static_assert(xarity<void (___test::*)(int, int)>::value == 3);
static_assert(xarity<void (___test::*)(int, int, int)>::value == 4);

static_assert(arity<std::function<void ()>>::value == 0);
static_assert(arity<std::function<void (int)>>::value == 1);
static_assert(arity<std::function<void (int, int)>>::value == 2);
static_assert(arity<std::function<void (int, int, int)>>::value == 3);

class ___test1 { ___test1() { auto l0 = []{}; auto l1 = [&](int){}; auto l2 = [=](int,int){}; auto l3 = [this](int,int,int){ (void)this; };
    static_assert(arity<decltype(l0)>::value == 0);
    static_assert(arity<decltype(l1)>::value == 1);
    static_assert(arity<decltype(l2)>::value == 2);
    static_assert(arity<decltype(l3)>::value == 3);
};};

/**
 * Returns the type of the function concrete argument.
 */
template <typename T, size_t i>
using arg_t = typename details<T>::template arg_t<i>;

template <typename T, size_t i>
using xarg_t = typename details<T>::template xarg_t<i>;

static_assert(std::is_same_v<arg_t<void (int, float), 0>, int>);
static_assert(std::is_same_v<arg_t<void (int, float), 1>, float>);
static_assert(std::is_same_v<arg_t<void (const int, float), 0>, int>);
static_assert(std::is_same_v<arg_t<void (int, const float), 1>, float>);
static_assert(std::is_same_v<arg_t<void (int*, float), 0>, int*>);
static_assert(std::is_same_v<arg_t<void (int, const float*), 1>, const float*>);
static_assert(std::is_same_v<arg_t<void (int* const, float), 0>, int* >);
static_assert(std::is_same_v<arg_t<void (int, const float* const), 1>, const float*>);
static_assert(std::is_same_v<arg_t<void (int&, float), 0>, int&>);
static_assert(std::is_same_v<arg_t<void (int, const float&), 1>, const float&>);

/**
 * Returns the return type of a function.
 */
template <typename T>
using return_t = typename details<T>::return_t;

static_assert(std::is_same_v<return_t<int ()>, int>);
static_assert(std::is_same_v<return_t<float ()>, float>);
static_assert(std::is_same_v<return_t<int& ()>, int&>);
static_assert(std::is_same_v<return_t<const int& ()>, const int&>);
static_assert(std::is_same_v<return_t<const int* ()>, const int*>);
static_assert(std::is_same_v<return_t<const int* const ()>, const int* const>);

/**
 * Returns the class type of a member function.
 */
template <typename T>
using class_t = typename details<T>::class_t;

static_assert(std::is_same_v<class_t<void (___test::*) ()>, ___test>);

/**
 * Returns true if a function has no return value.
 */
template <typename F>
struct is_void : std::is_same<return_t<F>, void> { };

static_assert(is_void<void ()>::value);
static_assert(!is_void<int ()>::value);

} // namespace xt
