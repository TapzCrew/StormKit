target("stormkit-engine")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.nzsl2spv")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")
    add_files("src/**.nzsl")

    add_headerfiles("include/(stormkit/**.mpp)")
    add_headerfiles("src/**.mpp")

    add_deps("stormkit-core", "stormkit-image", "stormkit-gpu", "stormkit-entities", { public = true })
    add_deps("stormkit-log")

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator", { public = true })
    add_packages("nzsl")

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include")

    set_group("libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

includes("examples/**/xmake.lua")
