// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#define STRINGIFY(x) #x
#define CASE(x) \
    case x: return STRINGIFY(x);
