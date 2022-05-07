local use_cxx20_modules = false

local allowedmodes = {
    "debug",
    "release",
    "releasedbg",
    "check",
    "coverage",
    "asan",
    "lsan",
    "ubsan",
    "profile",
    "check",
    "minsizerel"
}

if not is_plat("windows") then
    table.insert(allowedmodes, "valgrind")
end

set_xmakever("2.6.3")
set_project("StormKit")

set_version("0.1.0", { build = "%Y%m%d%H%M" } )

add_rules("mode.debug",
          "mode.release",
          "mode.releasedbg",
          "mode.check",
          "mode.coverage",
          "mode.asan",
          "mode.lsan",
          "mode.tsan",
          "mode.ubsan",
          "mode.profile",
          "mode.check",
          "mode.minsizerel")

if not is_plat("windows") then
    add_rules("mode.valgrind")
end

set_allowedmodes(allowedmodes)
set_allowedplats("windows", "mingw", "linux", "macosx")
set_allowedarchs("windows|x64", "mingw|x86_64", "linux|x86_64", "macosx|x86_64")

package("StormKit")
    option("enable_cxx20_modules")
        set_default(use_cxx20_modules)
        set_showmenu(true)
        add_defines("STORMKIT_CXX20_MODULES")
    option_end()

    option("stl_cache_directory", { showmenu = true })

    option("enable_wsi")
        set_showmenu(true)
        add_defines("STORMKIT_WSI_ENABLED")
        set_default(true)
    option_end()

    option("enable_wsi_x11")
        set_showmenu(true)
        add_defines("STORMKIT_WSI_BUILD_X11")
        add_deps("enable_wsi")

        on_check(function(option)
            option:enable(false)
            if is_plat("linux") then
                option:enable(true)
            end
        end)

        after_check(function(option)
            if not is_plat("linux") then
                option:enable(false)
            end

            if not option:dep("enable_wsi"):enabled() then
                option:enable(false)
            end
        end)
    option_end()

    option("enable_wsi_wayland")
        set_showmenu(true)
        add_defines("STORMKIT_WSI_BUILD_WAYLAND")
        add_deps("enable_wsi")

        on_check(function(option)
            option:enable(false)
            if is_plat("linux") then
                option:enable(true)
            end
        end)

        after_check(function(option)
            if not is_plat("linux") then
                option:enable(false)
            end

            if not option:dep("enable_wsi"):enabled() then
                option:enable(false)
            end
        end)
    option_end()

    add_requires("robin-hood-hashing",
                 "mapbox_eternal",
                 "glm",
                 "libjpeg",
                 "libpng",
                 "gli",
                 "vulkan-headers",
                 "volk",
                 "vulkan-memory-allocator",
                 "backward-cpp")
    add_requires("volk", { configs = { header_only = true }})
    if not is_plat("windows") then
        add_requires("fmt")

        if is_plat("linux") then
            add_requires("libxkbcommon", { configs = { x11 = has_config("enable_wsi_x11"),
                                                       wayland = has_config("enable_wsi_wayland") } })

            if has_config("enable_wsi_x11") then
                add_requires("libxcb",
                             "xcb-util-keysyms",
                             "xcb-util",
                             "xcb-util-wm",
                             "xcb-util-errors")
            end

            if has_config("enable_wsi_wayland") then
                add_requires("wayland",
                             "wayland-protocols")
            end
        end
    end


    if is_plat("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")

        add_defines("_CRT_SECURE_NO_WARNINGS")
        add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:lambda", "/Zc:preprocessor", "/Zc:referenceBinding", "/Zc:strictStrings", "/Zc:throwingNew")
        add_cxflags("/w44062") -- Enable warning: switch case not handled
        add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah

        if not has_config("enable_cxx20_modules") then
            add_cxxflags("/experimental:module-")
        end
    end

    if has_config("enable_cxx20_modules") then
        add_defines("STORMKIT_CXX20_MODULES")
    else
        add_defines("module=")
        add_defines("_ALLOW_KEYWORD_MACROS")
    end

    if is_mode("release") then
       set_symbols("hidden")
    else
       set_symbols("debug", "hidden")
    end

    set_fpmodels("fast")

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
    end

    add_vectorexts("mms", "neon", "avx", "avx2", "sse", "sse2", "sse3", "sse4")

    includes("core/xmake.lua")
    includes("log/xmake.lua")
    includes("main/xmake.lua")
    includes("entities/xmake.lua")
    includes("image/xmake.lua")

    if has_config("enable_wsi") then
        includes("wsi/xmake.lua")
    end

    includes("gpu/xmake.lua")
