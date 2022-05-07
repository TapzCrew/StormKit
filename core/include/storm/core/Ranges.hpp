// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>


#if __has_include(<ranges>) && defined(__cpp_lib_concepts) && defined(__cpp_lib_ranges)
    #include <algorithm>
    #include <ranges>
namespace storm::core {
    namespace ranges = std::ranges;
} // namespace storm::core
#elif __has_include(<range/v3/all.hpp>) && !defined(STORMKIT_GEN_DOC)
    #if defined(STORMKIT_OS_MACOS) || defined(STORMKIT_GEN_DOC)
        #define __cpp_concepts_back __cpp_concepts
        #define __cpp_impl_three_way_comparison_back __cpp_impl_three_way_comparison
        #define OLD_Nil Nil

        #undef __cpp_concepts
        #undef __cpp_impl_three_way_comparison
        #undef Nil
    #endif

    #include <range/v3/all.hpp>
namespace storm::core {
    namespace ranges = ranges::cpp20;
} // namespace storm::core
    #if defined(STORMKIT_OS_MACOS) || defined(STORMKIT_GEN_DOC)
        #define __cpp_concepts __cpp_concepts_back
        #define __cpp_impl_three_way_comparison __cpp_impl_three_way_comparison_back
        #define Nil OLD_Nil

        #undef __cpp_concepts_back
        #undef __cpp_impl_three_way_comparison_back
        #undef OLD_Nil
    #endif
#endif
