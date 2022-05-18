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

    add_headerfiles("src/common/**.mpp")
    add_headerfiles("src/*.mpp")
    add_headerfiles("include/(stormkit/**.mpp)")

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
    add_includedirs("$(buildir)/include")

    add_options("enable_wsi_x11")
    add_options("enable_wsi_wayland")

    set_group("libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

includes("examples/**/xmake.lua")
