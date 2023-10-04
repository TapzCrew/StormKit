// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.Core;

import Test;

using namespace stormkit::core;

#define expects(x) test::expects(x, #x)

namespace {
    auto is_equal = test::TestSuite {
        "Core.Numerics",
        {
            { "isEqualSameFloatT",
              [] {
                  expects((isEqual<float>(0.1f, 0.1f)));
                  expects((isEqual<double>(0.1, 0.1)));
                  expects((isEqual<long double>(0.1l, 0.1l)));

                  expects((not isEqual<float>(0.1f, 0.2f)));
                  expects((not isEqual<double>(0.1, 0.2)));
                  expects((not isEqual<long double>(0.1l, 0.2l)));

                  expects((isEqual(0.1f, 0.1f)));
                  expects((isEqual(0.1, 0.1)));
                  expects((isEqual(0.1l, 0.1l)));

                  expects((not isEqual(0.1f, 0.2f)));
                  expects((not isEqual(0.1, 0.2)));
                  expects((not isEqual(0.1l, 0.2l)));
              } },
            { "IsEqualDifferentFloatT",
              [] {
                  const auto float1 = 0.1f;
                  const auto float2 = 0.2f;

                  const auto double1 = 0.1;
                  const auto double2 = 0.2;

                  const auto longdouble1 = 0.1l;
                  const auto longdouble2 = 0.2l;

                  expects((isEqual<float, double>(float1, double1)));
                  expects((isEqual<double, float>(double1, float1)));
                  expects((isEqual<float, long double>(float1, longdouble1)));
                  expects((isEqual<long double, float>(longdouble1, float1)));
                  expects((isEqual<double, long double>(double1, longdouble1)));
                  expects((isEqual<long double, double>(longdouble1, double1)));

                  expects((not isEqual<float, double>(float1, double2)));
                  expects((not isEqual<double, float>(double2, float1)));
                  expects((not isEqual<float, double>(float2, double1)));
                  expects((not isEqual<double, float>(double1, float2)));
                  expects((not isEqual<float, long double>(float1, longdouble2)));
                  expects((not isEqual<long double, float>(longdouble2, float1)));
                  expects((not isEqual<float, long double>(float2, longdouble1)));
                  expects((not isEqual<long double, float>(longdouble1, float2)));
                  expects((not isEqual<double, long double>(double1, longdouble2)));
                  expects((not isEqual<long double, double>(longdouble2, double1)));
                  expects((not isEqual<double, long double>(double2, longdouble1)));
                  expects((not isEqual<long double, double>(longdouble1, double2)));

                  expects((isEqual(float1, double1)));
                  expects((isEqual(double1, float1)));
                  expects((isEqual(float1, longdouble1)));
                  expects((isEqual(longdouble1, float1)));
                  expects((isEqual(double1, longdouble1)));
                  expects((isEqual(longdouble1, double1)));

                  expects((not isEqual(float1, double2)));
                  expects((not isEqual(double2, float1)));
                  expects((not isEqual(float2, double1)));
                  expects((not isEqual(double1, float2)));
                  expects((not isEqual(float1, longdouble2)));
                  expects((not isEqual(longdouble2, float1)));
                  expects((not isEqual(float2, longdouble1)));
                  expects((not isEqual(longdouble1, float2)));
                  expects((not isEqual(double1, longdouble2)));
                  expects((not isEqual(longdouble2, double1)));
                  expects((not isEqual(double2, longdouble1)));
                  expects((not isEqual(longdouble1, double2)));
              } },
            { "isEqualSameIntegerT", [] {} },

        }
    };
}
