// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#pragma once

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>

/////////// - XKB - ///////////
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

#define STORMKIT_XKB_SCOPED(name, type)                                  \
    struct STORMKIT_PUBLIC name##Scoped {                                \
        name##Scoped() noexcept = default;                               \
        name##Scoped(type *handle) noexcept;                             \
        ~name##Scoped() noexcept;                                        \
                                                                         \
        name##Scoped(const name##Scoped &) = delete;                     \
        auto operator=(const name##Scoped &) -> name##Scoped & = delete; \
                                                                         \
        name##Scoped(name##Scoped &&) noexcept;                          \
        auto operator=(name##Scoped &&) noexcept -> name##Scoped &;      \
                                                                         \
        operator type *() const noexcept { return m_handle; }            \
                                                                         \
      private:                                                           \
        type *m_handle = nullptr;                                        \
    }

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.details.xkb;

// clang-format off
/////////// - StormKit::wsi - ///////////
import stormkit.wsi.key;
// clang-format on
export {
#else
    /////////// - StormKit::wsi - ///////////
    #include <stormkit/wsi/Key.mpp>
#endif

    namespace stormkit::wsi::details {
        STORMKIT_RAII_CAPSULE(XKBKeymap, xkb_keymap, xkb_keymap_unref)
        STORMKIT_RAII_CAPSULE(XKBState, xkb_state, xkb_state_unref)

        STORMKIT_XKB_SCOPED(XKBContext, xkb_context);

        struct XKBMods {
            xkb_mod_index_t shift;
            xkb_mod_index_t lock;
            xkb_mod_index_t control;
            xkb_mod_index_t mod1;
            xkb_mod_index_t mod2;
            xkb_mod_index_t mod3;
            xkb_mod_index_t mod4;
            xkb_mod_index_t mod5;
        };

        STORMKIT_PUBLIC auto getXKBContext() noexcept -> XKBContextScoped;

        STORMKIT_PUBLIC auto stormkitKeyToXKBKey(Key key) noexcept -> xkb_keysym_t;
        STORMKIT_PUBLIC auto xkbKeyToStormkitKey(xkb_keysym_t key) noexcept -> Key;
    } // namespace stormkit::wsi::details

#if defined(STORMKIT_CXX20_MODULES)
}
#endif

#undef STORMKIT_XKB_SCOPED
