// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

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

} // namespace xt
