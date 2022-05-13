target("stormkit-engine")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

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

    add_deps("stormkit-core", "stormkit-image", "stormkit-gpu", "stormkit-entities", { public = true })
    add_deps("stormkit-log")

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator", { public = true })

    add_includedirs("include", { public = true })

    add_options("enable_cxx20_modules")

    set_group("libraries")

includes("examples/**/xmake.lua")
