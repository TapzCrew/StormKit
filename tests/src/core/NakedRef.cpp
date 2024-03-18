// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.Core;

import Test;

using namespace stormkit::core;

#define expects(x) test::expects(x, #x)

namespace {
    auto _ = test::TestSuite {
        { { "Borrowed.toBorroweds", [] {
               {
                   auto vec  = std::vector { 1, 3, 5, 6, 9 };
                   auto refs = toBorroweds(vec);

                   auto i = 0;
                   for (auto&& ref : refs) expects(ref == vec[i++]);
               }
               {
                   auto vec  = std::array { 1, 3, 5, 6, 9 };
                   auto refs = toBorroweds(vec);

                   auto i = 0;
                   for (auto&& ref : refs) expects(ref == vec[i++]);
               }
               {
                   auto vec  = std::array { 1, 3, 5, 6, 9 };
                   auto refs = toBorroweds<std::array>(vec);

                   auto i = 0;
                   for (auto&& ref : refs) expects(ref == vec[i++]);
               }
           } } };
    };
} // namespace
