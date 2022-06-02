target("game_of_life")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.nzsl2spv")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_files("src/*.cpp")
    add_headerfiles("src/*.mpp")
    add_files("shaders/*.nzsl")
    
    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-entities", "stormkit-image", "stormkit-gpu")
    add_packages("nzsl")

    add_includedirs("$(buildir)/include")

    set_group("examples/stormkit-entities")
