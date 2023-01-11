target("uniform_buffer")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")

    add_packages("nzsl")
    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi", "stormkit-gpu")

    add_rules("stormkit.utils.nzsl2spv")

    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        add_defines("STORMKIT_ASSERT=1")
        set_suffixname("-d")
    else
        add_defines("STORMKIT_ASSERT=0")
    end

    if not has_config("use_modules") then
        add_defines("STORMKIT_NO_MODULES") -- for syntax highlighting
    else
        add_defines("STORMKIT_BUILD_MODULES")
    end

    add_files("src/*.cpp")
    add_moduleorheaderfiles("src/*.mpp")
    add_files("shaders/*.nzsl")
    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    set_group("examples/stormkit-gpu")




