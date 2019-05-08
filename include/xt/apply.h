// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "function_typeinfo.h"
#include "custom_exception.h"

#include <sstream>
#include <any>

namespace xt {

CUSTOM_EXCEPTION(NoEnoughArgumentsException, std::invalid_argument);
CUSTOM_EXCEPTION(TooManyArgumentsException, std::invalid_argument);

template <class T>
T convert(std::any any) {
    return std::any_cast<T>(any);
}

template <size_t N> void validate_apply_args(size_t size) {
    if (N > size) {
        std::stringstream ss;
        ss << "No enough arguments. Expected " << N << " got " << size;
        throw NoEnoughArgumentsException(ss.str());
    }

    if (N < size) {
        std::stringstream ss;
        ss << "Too many arguments. Expected " << N << " got " << size;
        throw TooManyArgumentsException(ss.str());
    }
}

template <typename C>
struct apply_impl {

    template <typename F, typename Iter, std::size_t... I>
    static decltype(auto) _(F&& f, Iter i, std::index_sequence<I...>, std::forward_iterator_tag) {
        return std::apply(std::forward<F>(f), std::tuple<C*, arg_t<F, I>...>{std::any_cast<C*>(*(i++)), convert<arg_t<F, I>>(((void)I, *(i++)))...});
    }

    template <typename F, typename Iter, std::size_t... I>
    static decltype(auto) _(F&& f, Iter i, std::index_sequence<I...>, std::random_access_iterator_tag) {
        return std::apply(std::forward<F>(f), std::tuple<C*, arg_t<F, I>...>{std::any_cast<C*>(*(i)), convert<arg_t<F, I>>(*(i+I+1))...});
    }
};

template <>
struct apply_impl<void> {

    template <typename F, typename Iter, std::size_t... I>
    static decltype(auto) _(F&& f, Iter i, std::index_sequence<I...>, std::forward_iterator_tag) {
        return std::apply(std::forward<F>(f), std::tuple<arg_t<F, I>...>{convert<arg_t<F, I>>(((void)I, *(i++)))...});
    }

    template <typename F, typename Iter, std::size_t... I>
    static decltype(auto) _(F&& f, Iter i, std::index_sequence<I...>, std::random_access_iterator_tag) {
        return std::apply(std::forward<F>(f), std::tuple<arg_t<F, I>...>{convert<arg_t<F, I>>(*(i+I))...});
    }
};

template <typename F, typename Iter, std::size_t... I, size_t N = arity<F>::value, typename Indices = std::make_index_sequence<N>>
decltype(auto) apply_i(F&& f, Iter&& i) {
    using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
    return apply_impl<class_t<F>>::_(std::forward<F>(f), std::forward<Iter>(i), Indices{}, iterator_category());
}

template <typename F, typename C, size_t N = xarity<F>::value>
decltype(auto) apply(F&& f, C&& c) {
    validate_apply_args<N>(c.size());
    return apply_i(std::forward<F>(f), c.begin());
}

template <typename F, size_t N = xarity<F>::value>
decltype(auto) apply(F&& f, std::initializer_list<std::any> c) {
    validate_apply_args<N>(c.size());
    return apply_i(std::forward<F>(f), c.begin());
}

} // namespace xt
