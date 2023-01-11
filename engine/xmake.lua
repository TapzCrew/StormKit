target("stormkit-engine")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")
    add_rules("stormkit.utils.nzsl2spv")

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        set_suffixname("-d")
    end

    add_files("include/stormkit/**.mpp")
    add_files("src/**.mpp")
    add_headerfiles("src/**.hpp")
    add_files("src/**.nzsl")
    add_files("src/**.cpp")

    add_deps("stormkit-core", "stormkit-image", "stormkit-gpu", "stormkit-entities", { public = true })
    add_deps("stormkit-log")

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator", { public = true })
    add_packages("nzsl")

    add_includedirs("$(buildir)/include")

    set_values("msvc.modules.stdifcdir", true)

    set_group("libraries")

    before_install(function(target)
        target:set("modulefiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

if has_config("enable_examples") then
    includes("examples/**/xmake.lua")
end


if has_config("enable_applications") then
    includes("applications/**/xmake.lua")
end
