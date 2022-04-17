// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto DebugGUI::setSkipFrameCount(core::UInt32 count) noexcept -> void {
        m_skip_frame = count;
    }
} // namespace storm::engine
