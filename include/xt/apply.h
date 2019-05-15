// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "function_typeinfo.h"
#include "custom_exception.h"

#include <sstream>
#include <any>
#include <typeindex>

namespace xt {

CUSTOM_EXCEPTION(NoEnoughArgumentsException, std::invalid_argument);
CUSTOM_EXCEPTION(TooManyArgumentsException, std::invalid_argument);

template <class T, typename = void>
struct convert {

    using cstref_t = std::add_lvalue_reference_t<std::add_const_t<T>>;

    T operator()(const std::any& any) {
        return std::any_cast<cstref_t>(any);
    }

    T operator()(cstref_t t) {
        return t;
    }
};

template <class T>
struct convert<T, std::enable_if_t<std::is_reference_v<T>> > {
    T operator()(T t) {
        return t;
    }

    T operator()(std::any&) {
        throw;
    }

    T operator()(const std::any&) {
        throw;
    }
};

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

template <typename F, size_t I, typename = void>
struct add_pointer_to_class_t : details<F>::template xarg<I> {};

template <typename F>
struct add_pointer_to_class_t<F, 0, std::enable_if_t<!std::is_same_v<class_t<F>, void>>> {
    using type = std::add_pointer_t<xarg_t<F, 0>>;
};

template <typename F, size_t I, typename T = xarg_t<F, I>, typename = void>
struct convert_impl {

    using val_t = std::remove_pointer_t<std::decay_t<T>>;

    struct _test_ {
        using cst_t = std::add_const_t<val_t>;
        static_assert(std::is_same_v<cst_t, const val_t>);
        using ptr_t = std::add_pointer_t<val_t>;
        static_assert(std::is_same_v<ptr_t, val_t*>);
        using cstptr_t = std::add_pointer_t<cst_t>;
        static_assert(std::is_same_v<cstptr_t, const val_t*>);
        using ref_t = std::add_lvalue_reference_t<val_t>;
        static_assert(std::is_same_v<ref_t, val_t&>);
        using cstref_t = std::add_lvalue_reference_t<cst_t>;
        static_assert(std::is_same_v<cstref_t, const val_t&>);
    };

    // Reference to T.
    T operator()(val_t& r) {
        return convert<T>()(r);
    }

    // Const reference to T.
    T operator()(const val_t& cr) {
        return convert<T>()(cr);
    }

    // Pointer to T.
    T operator()(val_t* p) {
        return p;
    }

    // Const pointer to T.
    T operator()(const val_t* cp) {
        return cp;
    }

    // For std::any types.
    using ref_t = std::add_lvalue_reference_t<T>;
    using cstref_t = std::add_lvalue_reference_t<std::add_const_t<T>>;

    T operator()(std::any& any) {
        if (std::type_index(any.type()) == std::type_index(typeid(T)))
            return convert_impl<F, I, T>()(std::any_cast<ref_t>(any));
        return convert<T>()(any);
    }

    T operator()(const std::any& any) {
        if (std::type_index(any.type()) == std::type_index(typeid(T)))
            return convert_impl<F, I, T>()(std::any_cast<cstref_t>(any));
        return convert<T>()(any);
    }
};

// Spacialization for 'this' of member functions.
template <typename F, typename P>
struct convert_impl<F, 0, P, std::enable_if_t<!std::is_same_v<class_t<F>, void>>> {

    using cls_t = class_t<F>;

    struct _test_ {
        using clsref_t = std::add_lvalue_reference_t<cls_t>;
        static_assert(std::is_same_v<clsref_t, cls_t&>);
        using clsconstref_t = std::add_lvalue_reference_t<std::add_const_t<cls_t>>;
        static_assert(std::is_same_v<clsconstref_t, const cls_t&>);
        using clsptr_t = std::add_pointer_t<cls_t>;
        static_assert(std::is_same_v<clsptr_t, cls_t*>);
        static_assert(std::is_same_v<clsptr_t, P>);
        using clsconstptr_t = std::add_pointer_t<std::add_const_t<cls_t>>;
        static_assert(std::is_same_v<clsconstptr_t, const cls_t*>);
    };

    // Reference to pointer.
    cls_t* operator()(cls_t& r) {
        return &r;
    }

    // Const reference to const pointer.
    const cls_t* operator()(const cls_t& cr) {
        return &cr;
    }

    // Pointer to pointer.
    cls_t* operator()(cls_t* p) {
        return p;
    }

    // Const pointer to const pointer.
    const cls_t* operator()(const cls_t* cp) {
        return cp;
    }

    // For std::any types.
    cls_t* operator()(std::any& any) {
        if (std::type_index(any.type()) == std::type_index(typeid(cls_t)))
            return &(std::any_cast<cls_t&>(any));
        return convert_impl<void, std::numeric_limits<size_t>::max(), P>()(any);
    }

    //!!! for const std::any& any
};

template <typename F, typename It, std::size_t... I>
static decltype(auto) apply_impl(F&& f, It&& i, std::index_sequence<I...>, std::input_iterator_tag) {
    using tuple_t = std::tuple<decltype(convert_impl<F, I>()(*i))...>;
    return std::apply(std::forward<F>(f), tuple_t{convert_impl<F, I>()(*(I ? ++i : i))...});
}

template <typename F, typename It, std::size_t... I>
static decltype(auto) apply_impl(F&& f, It&& i, std::index_sequence<I...>, std::random_access_iterator_tag) {
    using tuple_t = std::tuple<decltype(convert_impl<F, I>()(*i))...>;
    return std::apply(std::forward<F>(f), tuple_t{convert_impl<F, I>()(*(i+I))...});
}


template <typename F, typename It, std::size_t... I, size_t N = xarity<F>::value, typename Indices = std::make_index_sequence<N>>
decltype(auto) apply_i(F&& f, It&& i) {
    using iterator_category = typename std::iterator_traits<It>::iterator_category;
    return apply_impl(std::forward<F>(f), std::forward<It>(i), Indices{}, iterator_category());
}

template <typename F, typename C, size_t N = xarity<F>::value>
decltype(auto) apply(F&& f, C&& c) {
    validate_apply_args<N>(c.size());
    return apply_i(std::forward<F>(f), c.begin());
}

template <typename F, typename T, size_t N = xarity<F>::value>
decltype(auto) apply(F&& f, std::initializer_list<T> c) {
    validate_apply_args<N>(c.size());
    return apply_i(std::forward<F>(f), c.begin());
}

} // namespace xt
