// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace xt {

template <typename F, typename C, size_t N = arity<F>::value, typename Indices = std::make_index_sequence<N>>
decltype(auto) apply(F&& f, C&& c) {
    return apply_impl(std::forward<F>(f), std::forward<C>(c), Indices{});
}

} // namespace xt
