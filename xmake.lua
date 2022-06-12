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

add_repositories("tapzcrew-repo https://gitlab.com/tapzcrew/xmake-repo.git main")
add_repositories("nazara-engine-repo https://github.com/NazaraEngine/xmake-repo")

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
    option("unity_build")
        set_default(false)
        set_showmenu(true)
        set_category("root menu/misc")
    option_end()

    option("stl_cache_directory", { showmenu = true })

    option("enable_log")
        set_showmenu(true)
        set_default(true)
        set_category("root menu/modules")
    option_end()

    option("enable_image")
        set_showmenu(true)
        set_default(true)
        set_category("root menu/modules")
    option_end()

    option("enable_gpu")
        set_showmenu(true)
        set_default(true)
        set_category("root menu/modules")
    option_end()

    option("enable_entities")
        set_showmenu(true)
        set_default(true)
        add_deps("enable_log")
        add_deps("enable_image")
        set_category("root menu/modules")
    option_end()


    option("enable_wsi")
        set_showmenu(true)
        add_defines("STORMKIT_WSI_ENABLED")
        set_default(true)
        add_deps("enable_log")
        set_category("root menu/modules")
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

        set_category("root menu/modules_options")
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

        set_category("root menu/modules_options")
    option_end()

    option("enable_engine")
        set_showmenu(true)
        set_default(true)
        set_category("root menu/modules")
        add_deps("enable_log")
        add_deps("enable_image")
        add_deps("enable_gpu")
        add_deps("enable_entities")
        add_deps("enable_wsi")
    option_end()

    if is_plat("windows") then
        set_runtimes(is_mode("debug") and "MDd" or "MD")
        add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", { public = true })

        add_defines("_CRT_SECURE_NO_WARNINGS")
        add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:lambda", "/Zc:preprocessor", "/Zc:referenceBinding", "/Zc:strictStrings", "/Zc:throwingNew")
        add_cxflags("/w44062") -- Enable warning: switch case not handled
        add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
        add_cxflags("/wd4297")

        if not has_config("enable_cxx20_modules") then
            add_cxxflags("/experimental:module-")
        end
    end

    if has_config("unity_build") then
        add_rules("c.unity_build", { batchsize = 12 })
        add_rules("c++.unity_build", { batchsize = 12 })
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

    -- core deps
    add_requires("robin-hood-hashing",
                 "frozen",
                 "glm",
                 "backward-cpp")

    -- images deps
    if has_config("enable_image") then
        add_requires("libjpeg",
                     "libpng",
                     "gli")
    end

    -- render deps
    if has_config("enable_render") then
        add_requires("vulkan-headers",
                     "vulkan-memory-allocator",
                     "nzsl")
        add_requires("volk", { configs = { header_only = true }})
    end
    if not is_plat("windows") then
        add_requires("fmt")

        if is_plat("linux") and has_config("enable_wsi") then
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

    includes("xmake/**.lua")

    includes("core/xmake.lua")
    includes("main/xmake.lua")

    if has_config("enable_log") then
        includes("log/xmake.lua")
    end

    if has_config("enable_entities") then
        includes("entities/xmake.lua")
    end

    if has_config("enable_image") then
        includes("image/xmake.lua")
    end

    if has_config("enable_wsi") then
        includes("wsi/xmake.lua")
    end

    if has_config("enable_gpu") then
        includes("gpu/xmake.lua")
    end

    if has_config("enable_engine") then
    includes("engine/xmake.lua")
    end
