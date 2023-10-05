// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.Core;

import Test;

using namespace stormkit::core;

#define expects(x) test::expects(x, #x)

namespace {
    using schar        = signed char;
    using uchar        = unsigned char;
    using sshort       = signed short;
    using ushort       = unsigned short;
    using sint         = signed int;
    using uint         = unsigned int;
    using slongint     = signed long int;
    using ulongint     = unsigned long int;
    using slonglongint = signed long long int;
    using ulonglongint = unsigned long long int;

    const auto char1 = char { 1 };
    const auto char2 = char { 2 };

    const auto signed_char1 = schar { 1 };
    const auto signed_char2 = schar { 2 };

    const auto unsigned_char1 = uchar { 1 };
    const auto unsigned_char2 = uchar { 2 };

    const auto signed_short1 = sshort { 1 };
    const auto signed_short2 = sshort { 2 };

    const auto unsigned_short1 = ushort { 1 };
    const auto unsigned_short2 = ushort { 2 };

    const auto signed_int1 = sint { 1 };
    const auto signed_int2 = sint { 2 };

    const auto unsigned_int1 = uint { 1 };
    const auto unsigned_int2 = uint { 2 };

    const auto signed_longint1 = slongint { 1 };
    const auto signed_longint2 = slongint { 2 };

    const auto unsigned_longint1 = ulongint { 1 };
    const auto unsigned_longint2 = ulongint { 2 };

    const auto signed_longlongint1 = slonglongint { 1 };
    const auto signed_longlongint2 = slonglongint { 2 };

    const auto unsigned_longlongint1 = ulonglongint { 1 };
    const auto unsigned_longlongint2 = ulonglongint { 2 };

    auto _ = test::TestSuite {
        "Core.Numerics",
        { { "isEqual.SameFloatT",
            [] {
                auto float1 = 0.1f;
                auto float2 = 0.2f;

                auto double1 = 0.1;
                auto double2 = 0.2;

                auto longdouble1 = 0.1l;
                auto longdouble2 = 0.2l;

                expects((isEqual(float1, float1)));
                expects((isEqual(double1, double1)));
                expects((isEqual(longdouble1, longdouble1)));

                expects((not isEqual(float1, float2)));
                expects((not isEqual(float1, double2)));
                expects((not isEqual(longdouble1, longdouble2)));

                float1 = 100.1f;
                float2 = 100.2f;

                double1 = 1000.1;
                double2 = 1000.2;

                longdouble1 = 10000.1l;
                longdouble2 = 10000.2l;

                expects((isEqual(float1, float1)));
                expects((isEqual(double1, double1)));
                expects((isEqual(longdouble1, longdouble1)));

                expects((not isEqual(float1, float2)));
                expects((not isEqual(double1, double2)));
                expects((not isEqual(longdouble1, longdouble2)));
            } },
          { "IsEqual.DifferentFloatT",
            [] {
                auto float1 = 0.1f;
                auto float2 = 0.2f;

                auto double1 = 0.1;
                auto double2 = 0.2;

                auto longdouble1 = 0.1l;
                auto longdouble2 = 0.2l;

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

                float1 = 1000.1f;
                float2 = 1000.2f;

                double1 = 1000.1;
                double2 = 1000.2;

                longdouble1 = 1000.1l;
                longdouble2 = 1000.2l;

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
          { "isEqual.SameIntegerT",
            [] {
                expects((isEqual(char1, char1)));

                expects((isEqual(signed_char1, signed_char1)));
                expects((isEqual(unsigned_char1, unsigned_char1)));

                expects((isEqual(signed_short1, signed_short1)));
                expects((isEqual(unsigned_short1, unsigned_short1)));

                expects((isEqual(signed_int1, signed_int1)));
                expects((isEqual(unsigned_int1, unsigned_int1)));

                expects((isEqual(signed_longint1, signed_longint1)));
                expects((isEqual(unsigned_longint1, unsigned_longint1)));

                expects((isEqual(signed_longlongint1, signed_longlongint1)));
                expects((isEqual(unsigned_longlongint1, unsigned_longlongint1)));

                expects((not isEqual(char1, char2)));

                expects((not isEqual(signed_char1, signed_char2)));
                expects((not isEqual(unsigned_char1, unsigned_char2)));

                expects((not isEqual(signed_short1, signed_short2)));
                expects((not isEqual(unsigned_short1, unsigned_short2)));

                expects((not isEqual(signed_int1, signed_int2)));
                expects((not isEqual(unsigned_int1, unsigned_int2)));

                expects((not isEqual(signed_longint1, signed_longint2)));
                expects((not isEqual(unsigned_longint1, unsigned_longint2)));

                expects((not isEqual(signed_longlongint1, signed_longlongint2)));
                expects((not isEqual(unsigned_longlongint1, unsigned_longlongint2)));
            } },
          { "isEqual.DifferentIntegerT",
            [] {
                // char
                expects((isEqual(char1, signed_char1)));
                expects((isEqual(char1, unsigned_char1)));

                expects((isEqual(char1, signed_short1)));
                expects((isEqual(char1, unsigned_short1)));

                expects((isEqual(char1, signed_int1)));
                expects((isEqual(char1, unsigned_int1)));

                expects((isEqual(char1, signed_longint1)));
                expects((isEqual(char1, unsigned_longint1)));

                expects((isEqual(char1, signed_longlongint1)));
                expects((isEqual(char1, unsigned_longlongint1)));

                // signed char
                expects((isEqual(signed_char1, char1)));
                expects((isEqual(signed_char1, unsigned_char1)));

                expects((isEqual(signed_char1, signed_short1)));
                expects((isEqual(signed_char1, unsigned_short1)));

                expects((isEqual(signed_char1, signed_int1)));
                expects((isEqual(signed_char1, unsigned_int1)));

                expects((isEqual(signed_char1, signed_longint1)));
                expects((isEqual(signed_char1, unsigned_longint1)));

                expects((isEqual(signed_char1, signed_longlongint1)));
                expects((isEqual(signed_char1, unsigned_longlongint1)));

                // unsigned char
                expects((isEqual(unsigned_char1, char1)));
                expects((isEqual(unsigned_char1, signed_char1)));

                expects((isEqual(unsigned_char1, signed_short1)));
                expects((isEqual(unsigned_char1, unsigned_short1)));

                expects((isEqual(unsigned_char1, signed_int1)));
                expects((isEqual(unsigned_char1, unsigned_int1)));

                expects((isEqual(unsigned_char1, signed_longint1)));
                expects((isEqual(unsigned_char1, unsigned_longint1)));

                expects((isEqual(unsigned_char1, signed_longlongint1)));
                expects((isEqual(unsigned_char1, unsigned_longlongint1)));

                // signed short
                expects((isEqual(signed_short1, char1)));

                expects((isEqual(signed_short1, signed_char1)));
                expects((isEqual(signed_short1, unsigned_char1)));

                expects((isEqual(signed_short1, unsigned_short1)));

                expects((isEqual(signed_short1, signed_int1)));
                expects((isEqual(signed_short1, unsigned_int1)));

                expects((isEqual(signed_short1, signed_longint1)));
                expects((isEqual(signed_short1, unsigned_longint1)));

                expects((isEqual(signed_short1, signed_longlongint1)));
                expects((isEqual(signed_short1, unsigned_longlongint1)));

                // unsigned short
                expects((isEqual(unsigned_short1, char1)));
                expects((isEqual(unsigned_short1, signed_char1)));
                expects((isEqual(unsigned_short1, unsigned_char1)));

                expects((isEqual(unsigned_short1, signed_short1)));

                expects((isEqual(unsigned_short1, signed_int1)));
                expects((isEqual(unsigned_short1, unsigned_int1)));

                expects((isEqual(unsigned_short1, signed_longint1)));
                expects((isEqual(unsigned_short1, unsigned_longint1)));

                expects((isEqual(unsigned_short1, signed_longlongint1)));
                expects((isEqual(unsigned_short1, unsigned_longlongint1)));

                // signed int
                expects((isEqual(signed_int1, char1)));

                expects((isEqual(signed_int1, signed_char1)));
                expects((isEqual(signed_int1, unsigned_char1)));

                expects((isEqual(signed_int1, signed_short1)));
                expects((isEqual(signed_int1, unsigned_short1)));

                expects((isEqual(signed_int1, unsigned_int1)));

                expects((isEqual(signed_int1, signed_longint1)));
                expects((isEqual(signed_int1, unsigned_longint1)));

                expects((isEqual(signed_int1, signed_longlongint1)));
                expects((isEqual(signed_int1, unsigned_longlongint1)));

                // unsigned int
                expects((isEqual(unsigned_int1, char1)));
                expects((isEqual(unsigned_int1, signed_char1)));
                expects((isEqual(unsigned_int1, unsigned_char1)));

                expects((isEqual(unsigned_int1, signed_short1)));
                expects((isEqual(unsigned_int1, unsigned_short1)));

                expects((isEqual(unsigned_int1, signed_int1)));

                expects((isEqual(unsigned_int1, signed_longint1)));
                expects((isEqual(unsigned_int1, unsigned_longint1)));

                expects((isEqual(unsigned_int1, signed_longlongint1)));
                expects((isEqual(unsigned_int1, unsigned_longlongint1)));

                // signed longint
                expects((isEqual(signed_longint1, char1)));

                expects((isEqual(signed_longint1, signed_char1)));
                expects((isEqual(signed_longint1, unsigned_char1)));

                expects((isEqual(signed_longint1, signed_short1)));
                expects((isEqual(signed_longint1, unsigned_short1)));

                expects((isEqual(signed_longint1, unsigned_int1)));

                expects((isEqual(signed_longint1, unsigned_longint1)));

                expects((isEqual(signed_longint1, signed_longlongint1)));
                expects((isEqual(signed_longint1, unsigned_longlongint1)));

                // unsigned longint
                expects((isEqual(unsigned_longint1, char1)));
                expects((isEqual(unsigned_longint1, signed_char1)));
                expects((isEqual(unsigned_longint1, unsigned_char1)));

                expects((isEqual(unsigned_longint1, signed_short1)));
                expects((isEqual(unsigned_longint1, unsigned_short1)));

                expects((isEqual(unsigned_longint1, signed_int1)));
                expects((isEqual(unsigned_longint1, unsigned_int1)));

                expects((isEqual(unsigned_longint1, signed_longint1)));

                expects((isEqual(unsigned_longint1, signed_longlongint1)));
                expects((isEqual(unsigned_longint1, unsigned_longlongint1)));

                // signed longlongint
                expects((isEqual(signed_longlongint1, char1)));

                expects((isEqual(signed_longlongint1, signed_char1)));
                expects((isEqual(signed_longlongint1, unsigned_char1)));

                expects((isEqual(signed_longlongint1, signed_short1)));
                expects((isEqual(signed_longlongint1, unsigned_short1)));

                expects((isEqual(signed_longlongint1, unsigned_int1)));

                expects((isEqual(signed_longlongint1, signed_longint1)));
                expects((isEqual(signed_longlongint1, unsigned_longint1)));

                expects((isEqual(signed_longlongint1, unsigned_longlongint1)));

                // unsigned longlongint
                expects((isEqual(unsigned_longlongint1, char1)));
                expects((isEqual(unsigned_longlongint1, signed_char1)));
                expects((isEqual(unsigned_longlongint1, unsigned_char1)));

                expects((isEqual(unsigned_longlongint1, signed_short1)));
                expects((isEqual(unsigned_longlongint1, unsigned_short1)));

                expects((isEqual(unsigned_longlongint1, signed_int1)));
                expects((isEqual(unsigned_longlongint1, unsigned_int1)));

                expects((isEqual(unsigned_longlongint1, signed_longint1)));
                expects((isEqual(unsigned_longlongint1, unsigned_longint1)));

                expects((isEqual(unsigned_longlongint1, signed_longlongint1)));

                // char
                expects((not isEqual(char1, signed_char2)));
                expects((not isEqual(char1, unsigned_char2)));

                expects((not isEqual(char1, signed_short2)));
                expects((not isEqual(char1, unsigned_short2)));

                expects((not isEqual(char1, signed_int2)));
                expects((not isEqual(char1, unsigned_int2)));

                expects((not isEqual(char1, signed_longint2)));
                expects((not isEqual(char1, unsigned_longint2)));

                expects((not isEqual(char1, signed_longlongint2)));
                expects((not isEqual(char1, unsigned_longlongint2)));

                // signed char
                expects((not isEqual(signed_char1, char2)));
                expects((not isEqual(signed_char1, unsigned_char2)));

                expects((not isEqual(signed_char1, signed_short2)));
                expects((not isEqual(signed_char1, unsigned_short2)));

                expects((not isEqual(signed_char1, signed_int2)));
                expects((not isEqual(signed_char1, unsigned_int2)));

                expects((not isEqual(signed_char1, signed_longint2)));
                expects((not isEqual(signed_char1, unsigned_longint2)));

                expects((not isEqual(signed_char1, signed_longlongint2)));
                expects((not isEqual(signed_char1, unsigned_longlongint2)));

                // unsigned char
                expects((not isEqual(unsigned_char1, char2)));
                expects((not isEqual(unsigned_char1, signed_char2)));

                expects((not isEqual(unsigned_char1, signed_short2)));
                expects((not isEqual(unsigned_char1, unsigned_short2)));

                expects((not isEqual(unsigned_char1, signed_int2)));
                expects((not isEqual(unsigned_char1, unsigned_int2)));

                expects((not isEqual(unsigned_char1, signed_longint2)));
                expects((not isEqual(unsigned_char1, unsigned_longint2)));

                expects((not isEqual(unsigned_char1, signed_longlongint2)));
                expects((not isEqual(unsigned_char1, unsigned_longlongint2)));

                // signed short
                expects((not isEqual(signed_short1, char2)));

                expects((not isEqual(signed_short1, signed_char2)));
                expects((not isEqual(signed_short1, unsigned_char2)));

                expects((not isEqual(signed_short1, unsigned_short2)));

                expects((not isEqual(signed_short1, signed_int2)));
                expects((not isEqual(signed_short1, unsigned_int2)));

                expects((not isEqual(signed_short1, signed_longint2)));
                expects((not isEqual(signed_short1, unsigned_longint2)));

                expects((not isEqual(signed_short1, signed_longlongint2)));
                expects((not isEqual(signed_short1, unsigned_longlongint2)));

                // unsigned short
                expects((not isEqual(unsigned_short1, char2)));
                expects((not isEqual(unsigned_short1, signed_char2)));
                expects((not isEqual(unsigned_short1, unsigned_char2)));

                expects((not isEqual(unsigned_short1, signed_short2)));

                expects((not isEqual(unsigned_short1, signed_int2)));
                expects((not isEqual(unsigned_short1, unsigned_int2)));

                expects((not isEqual(unsigned_short1, signed_longint2)));
                expects((not isEqual(unsigned_short1, unsigned_longint2)));

                expects((not isEqual(unsigned_short1, signed_longlongint2)));
                expects((not isEqual(unsigned_short1, unsigned_longlongint2)));

                // signed int
                expects((not isEqual(signed_int1, char2)));

                expects((not isEqual(signed_int1, signed_char2)));
                expects((not isEqual(signed_int1, unsigned_char2)));

                expects((not isEqual(signed_int1, signed_short2)));
                expects((not isEqual(signed_int1, unsigned_short2)));

                expects((not isEqual(signed_int1, unsigned_int2)));

                expects((not isEqual(signed_int1, signed_longint2)));
                expects((not isEqual(signed_int1, unsigned_longint2)));

                expects((not isEqual(signed_int1, signed_longlongint2)));
                expects((not isEqual(signed_int1, unsigned_longlongint2)));

                // unsigned int
                expects((not isEqual(unsigned_int1, char2)));
                expects((not isEqual(unsigned_int1, signed_char2)));
                expects((not isEqual(unsigned_int1, unsigned_char2)));

                expects((not isEqual(unsigned_int1, signed_short2)));
                expects((not isEqual(unsigned_int1, unsigned_short2)));

                expects((not isEqual(unsigned_int1, signed_int2)));

                expects((not isEqual(unsigned_int1, signed_longint2)));
                expects((not isEqual(unsigned_int1, unsigned_longint2)));

                expects((not isEqual(unsigned_int1, signed_longlongint2)));
                expects((not isEqual(unsigned_int1, unsigned_longlongint2)));

                // signed longint
                expects((not isEqual(signed_longint1, char2)));

                expects((not isEqual(signed_longint1, signed_char2)));
                expects((not isEqual(signed_longint1, unsigned_char2)));

                expects((not isEqual(signed_longint1, signed_short2)));
                expects((not isEqual(signed_longint1, unsigned_short2)));

                expects((not isEqual(signed_longint1, unsigned_int2)));

                expects((not isEqual(signed_longint1, unsigned_longint2)));

                expects((not isEqual(signed_longint1, signed_longlongint2)));
                expects((not isEqual(signed_longint1, unsigned_longlongint2)));

                // unsigned longint
                expects((not isEqual(unsigned_longint1, char2)));
                expects((not isEqual(unsigned_longint1, signed_char2)));
                expects((not isEqual(unsigned_longint1, unsigned_char2)));

                expects((not isEqual(unsigned_longint1, signed_short2)));
                expects((not isEqual(unsigned_longint1, unsigned_short2)));

                expects((not isEqual(unsigned_longint1, signed_int2)));
                expects((not isEqual(unsigned_longint1, unsigned_int2)));

                expects((not isEqual(unsigned_longint1, signed_longint2)));

                expects((not isEqual(unsigned_longint1, signed_longlongint2)));
                expects((not isEqual(unsigned_longint1, unsigned_longlongint2)));

                // signed longlongint
                expects((not isEqual(signed_longlongint1, char2)));

                expects((not isEqual(signed_longlongint1, signed_char2)));
                expects((not isEqual(signed_longlongint1, unsigned_char2)));

                expects((not isEqual(signed_longlongint1, signed_short2)));
                expects((not isEqual(signed_longlongint1, unsigned_short2)));

                expects((not isEqual(signed_longlongint1, unsigned_int2)));

                expects((not isEqual(signed_longlongint1, signed_longint2)));
                expects((not isEqual(signed_longlongint1, unsigned_longint2)));

                expects((not isEqual(signed_longlongint1, unsigned_longlongint2)));

                // unsigned longlongint
                expects((not isEqual(unsigned_longlongint1, char2)));
                expects((not isEqual(unsigned_longlongint1, signed_char2)));
                expects((not isEqual(unsigned_longlongint1, unsigned_char2)));

                expects((not isEqual(unsigned_longlongint1, signed_short2)));
                expects((not isEqual(unsigned_longlongint1, unsigned_short2)));

                expects((not isEqual(unsigned_longlongint1, signed_int2)));
                expects((not isEqual(unsigned_longlongint1, unsigned_int2)));

                expects((not isEqual(unsigned_longlongint1, signed_longint2)));
                expects((not isEqual(unsigned_longlongint1, unsigned_longint2)));

                expects((not isEqual(unsigned_longlongint1, signed_longlongint2)));
            } },
          { "map", [] { expects((map(10, 0, 20, 0, 10) == 5)); } },
          { "range.simple",
            [] {
                auto rng     = range(10);
                auto success = true;
                for (auto i = 0; i < 10; ++i) {
                    if (rng[i] != i) {
                        success = false;
                        break;
                    }
                }
                expects(success);
            } },
          { "range.custom_start",
            [] {
                auto rng     = range(2, 10);
                auto success = true;
                for (auto i = 2; i < 10; ++i) {
                    if (rng[i - 2] != i) {
                        success = false;
                        break;
                    }
                }
                expects(success);
            } },
          { "range.custom_negative_start",
            [] {
                auto rng     = range(-5, 10);
                auto success = true;
                for (auto i = -5; i < 10; ++i) {
                    if (rng[i + 5] != i) {
                        success = false;
                        break;
                    }
                }
                expects(success);
            } },
          { "range.custom_step",
            [] {
                auto rng     = range(-6, 10, 2);
                auto success = true;
                auto j       = -6;
                for (auto i = 0u; i < std::size(rng); ++i) {
                    if (rng[i] != j) {
                        success = false;
                        break;
                    }
                    j += 2;
                }
                expects(success);
            } },
          { "range.custom_numeric_range",
            [] {
                auto rng     = range(NumericsRange { -5, 10, 2 });
                auto success = true;
                auto j       = -5;
                for (auto i = 0u; i < std::size(rng); ++i) {
                    if (rng[i] != j) {
                        success = false;
                        break;
                    }
                    j += 2;
                }
                expects(success);
            } },
          { "multirange.simple",
            [] {
                auto success = true;
                auto i       = 0;
                auto j       = 0;

                for (auto [v1, v2] : multiRange(10, 5)) {
                    if (v1 != i or v2 != j) {
                        success = false;
                        break;
                    }

                    j += 1;
                    if (j >= 5) {
                        j = 0;
                        i += 1;
                    }
                }

                expects(success);
            } },
          { "multirange.different_types",
            [] {
                auto success = true;
                auto i       = 0;
                auto j       = 0;

                for (auto [v1, v2] : multiRange(10, 5u)) {
                    if (v1 != i or v2 != j or not std::same_as<decltype(v2), decltype(j)>) {
                        success = false;
                        break;
                    }

                    j += 1u;
                    if (j >= 5) {
                        j = 0u;
                        i += 1;
                    }
                }

                expects(success);
            } },
          { "multirange.custom_start",
            [] {
                auto success = true;
                auto i       = 5;
                auto j       = 2;

                auto rng = multiRange(NumericsRange { 5, 10 }, NumericsRange { 2, 5 });
                for (auto [v1, v2] : rng) {
                    if (v1 != i or v2 != j) {
                        success = false;
                        break;
                    }

                    j += 1;
                    if (j >= 5) {
                        j = 2;
                        i += 1;
                    }
                }

                expects(success);
            } },
          { "multirange.custom_step",
            [] {
                auto success = true;
                auto i       = 0;
                auto j       = 0;

                auto rng = multiRange(NumericsRange { 0, 10, 5 }, NumericsRange { 0, 6, 2 });
                for (auto [v1, v2] : rng) {
                    if (v1 != i or v2 != j) {
                        success = false;
                        break;
                    }

                    j += 2;
                    if (j >= 5) {
                        j = 0;
                        i += 5;
                    }
                }

                expects(success);
            } } }
    };
} // namespace
