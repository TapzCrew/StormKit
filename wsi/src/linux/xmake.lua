target("stormkit-wsi-linux")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("*.inl")
    add_files("*.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("*.mpp")
    else
        add_headerfiles("*.mpp")
    end

    add_packages("libxkbcommon", {public = true})

    if has_config("enable_wsi_x11") then
        includes("x11/xmake.lua")
        add_deps("stormkit-wsi-linux-x11", {public = true})
    end

    if has_config("enable_wsi_wayland") then
        includes("wayland/xmake.lua")
        add_deps("stormkit-wsi-linux-wayland", {public = true})
        add_includedirs("$(buildir)/wayland_protocols")
    end

    add_includedirs("../../include/")
    add_deps("stormkit-core", "stormkit-log", { public = true })

    add_cxxflags("-fPIC")

    add_options("enable_cxx20_modules")
    set_group("private-libraries")
