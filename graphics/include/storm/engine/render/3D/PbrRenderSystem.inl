// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PbrRenderSystem::resetCamera() -> void { setCamera(m_default_camera); }
} // namespace storm::engine
