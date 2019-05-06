// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <tuple>

namespace xt {

template <typename R, typename... Args>
struct function_details {
    using return_t = R;

    enum { arity = sizeof...(Args) };

    using args_t = std::tuple<Args...>;

    template <size_t i>
    struct arg {
        using type = typename std::tuple_element<i, args_t>::type;
    };

    template <size_t i>
    using arg_t = typename arg<i>::type;
};

template <typename R, typename C, typename... Args>
struct method_details : function_details<R, Args...>{
    using class_t = C;
};

template <typename T>
struct details;

template <typename R, typename C, typename... Args>
struct details<R (C::*)(Args...)> : method_details<R, C, Args...> {};

template <typename R, typename C, typename... Args>
struct details<R (C::*)(Args...) const> : method_details<R, C, Args...> {};

template <typename R, typename C, typename... Args>
struct details<R (C::*)(Args..., ...)> : method_details<R, C, Args...> {};

template <typename R, typename C, typename... Args>
struct details<R (C::*)(Args..., ...) const> : method_details<R, C, Args...> {};

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

static_assert(std::is_same<details<void (___test::*)()>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::*)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* const)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* const)()>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* volatile)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* volatile)()>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::*)() const>::return_t, void>::value);

static_assert(std::is_same<details<void (___test::*&)()>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::*&)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* const &)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::* volatile &)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::*&)() const>::return_t, void>::value);
static_assert(std::is_same<details<void (___test::*&)()>::return_t, void>::value);

static_assert(std::is_same<details<void (*)()>::return_t, void>::value);
static_assert(std::is_same<details<void (* const)()>::return_t, void>::value);
static_assert(std::is_same<details<void (* volatile)()>::return_t, void>::value);

static_assert(std::is_same<details<void (*&)()>::return_t, void>::value);
static_assert(std::is_same<details<void (* const &)()>::return_t, void>::value);
static_assert(std::is_same<details<void (* volatile &)()>::return_t, void>::value);

template <typename T>
struct details : details<decltype(&T::operator())> {};

class ___test { ___test() { auto l = []{};
    static_assert(std::is_same<details<decltype(l)>::return_t, void>::value);
    static_assert(std::is_same<details<decltype(&l)>::return_t, void>::value);
    static_assert(std::is_same<details<decltype(l)&>::return_t, void>::value);
    static_assert(std::is_same<details<const decltype(l)&>::return_t, void>::value);
    static_assert(std::is_same<details<const decltype(l)*>::return_t, void>::value);
}};

/**
 * Returns the number of the function arguments.
 */
template <typename T>
struct arity {
    enum { value = details<T>::arity };
};

static_assert(arity<void (int, int)>::value == 2);
static_assert(arity<void (int, int, int)>::value == 3);
static_assert(arity<void (int, int, int, ...)>::value == 3);

/**
 * Returns the type of the function concrete argument.
 */
template <typename T, size_t i>
using arg_t = typename details<T>::template arg<i>::type;

static_assert(std::is_same<arg_t<void (int, float), 0>, int>::value);
static_assert(std::is_same<arg_t<void (int, float), 1>, float>::value);
static_assert(std::is_same<arg_t<void (const int, float), 0>, int>::value);
static_assert(std::is_same<arg_t<void (int, const float), 1>, float>::value);
static_assert(std::is_same<arg_t<void (int*, float), 0>, int*>::value);
static_assert(std::is_same<arg_t<void (int, const float*), 1>, const float*>::value);
static_assert(std::is_same<arg_t<void (int* const, float), 0>, int* >::value);
static_assert(std::is_same<arg_t<void (int, const float* const), 1>, const float*>::value);
static_assert(std::is_same<arg_t<void (int&, float), 0>, int&>::value);
static_assert(std::is_same<arg_t<void (int, const float&), 1>, const float&>::value);

/**
 * Returns the return type of a function.
 */
template <typename T>
using return_t = typename details<T>::return_t;

static_assert(std::is_same<return_t<int ()>, int>::value);
static_assert(std::is_same<return_t<float ()>, float>::value);
static_assert(std::is_same<return_t<int& ()>, int&>::value);
static_assert(std::is_same<return_t<const int& ()>, const int&>::value);
static_assert(std::is_same<return_t<const int* ()>, const int*>::value);
static_assert(std::is_same<return_t<const int* const ()>, const int* const>::value);

/**
 * Returns the class type of a member function.
 */
template <typename T>
using class_t = typename details<T>::class_t;

static_assert(std::is_same<class_t<void (___test::*) ()>, ___test>::value);

/**
 * Returns true if a function has no return value.
 */
template <typename F>
struct is_void : std::is_same<return_t<F>, void> { };

static_assert(is_void<void ()>::value);
static_assert(!is_void<int ()>::value);

} // namespace xt
