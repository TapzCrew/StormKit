target("stormkit-gpu")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("include/**.mpp")
        add_files("src/**.mpp")
    else
        add_headerfiles("include/(stormkit/**.mpp)")
        add_headerfiles("src/**.mpp")
    end

    add_deps("stormkit-core", "stormkit-image", { public = true })
    add_deps("stormkit-log")

    if has_config("enable_wsi") then
        add_deps("stormkit-wsi")

        add_defines("STORMKIT_WSI_ENABLED", { public = true })
    end

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator", { public = true })

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include")

    add_options("enable_cxx20_modules", "enable_wsi", "enable_wsi_x11", "enable_wsi_wayland")

    set_group("libraries")

includes("examples/**/xmake.lua")
