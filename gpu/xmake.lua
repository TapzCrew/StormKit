target("stormkit-gpu")
    set_kind("$(kind)")
    set_languages("cxx20", "clatest")

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")

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

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

if has_config("enable_examples") and has_config("enable_wsi") then
    includes("examples/**/xmake.lua")
end
