// Copyright (c) King Mampreh. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define CUSTOM_EXCEPTION(name, parentName)            \
    struct name : parentName {                        \
        explicit name(const std::string& s)           \
            : parentName(s)                           \
        {                                             \
        }                                             \
        explicit name(const char* s)                  \
            : parentName(s)                           \
        {                                             \
        }                                             \
};
