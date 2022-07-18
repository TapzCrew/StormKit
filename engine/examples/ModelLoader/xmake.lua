target("model_loader")
    set_kind("binary")
    set_languages("cxx20", "clatest")
    add_rules("utils.nzsl2spv")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_files("src/*.cpp")
    add_headerfiles("src/*.hpp")

    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-engine", "stormkit-log")
    add_packages("nzsl")

    add_includedirs("$(buildir)/include")

    set_group("examples/stormkit-engine")


