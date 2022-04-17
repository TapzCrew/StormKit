target("stormkit-render")
    set_kind("$(library_type)")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.inl")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("include/**.mpp")
        add_files("src/**.mpp")
    else
        add_headerfiles("include/**.mpp")
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

    add_options("enable_cxx20_modules", "enable_wsi", "enable_wsi_x11", "enable_wsi_wayland")

    set_group("libraries")

includes("examples/**/xmake.lua")
