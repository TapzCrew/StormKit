target("stormkit-wsi")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_headerfiles("src/*.hpp")
    add_headerfiles("src/common/**.hpp")
    add_headerfiles("src/common/**.inl")
    add_files("src/*.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("include/**.mpp")
        add_files("src/*.mpp")
        add_files("src/common/**.mpp")
    else
        add_headerfiles("src/common/**.mpp")
        add_headerfiles("src/*.mpp")
        add_headerfiles("include/(stormkit/**.mpp)")
    end

    if is_plat("windows") then
        includes("src/win32/xmake.lua")
        add_deps("stormkit-wsi-win32")
    elseif is_plat("linux") then
        includes("src/linux/xmake.lua")
        add_deps("stormkit-wsi-linux")
    elseif is_plat("macosx") then
    elseif is_plat("iphoneos") then
    elseif is_plat("android") then
    end

    add_deps("stormkit-core", "stormkit-log", { public = true })
    add_includedirs("include", { public = true })

    add_options("enable_cxx20_modules")
    add_options("enable_wsi_x11")
    add_options("enable_wsi_wayland")

    set_group("libraries")

includes("examples/**/xmake.lua")
