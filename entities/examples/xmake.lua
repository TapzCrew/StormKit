target("game_of_life")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.glsl2spv", {bin2c = true})

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_files("src/*.cpp")
    add_headerfiles("src/*.mpp")
    add_files("shaders/*.vert")
    add_files("shaders/*.frag")
    add_files("win32/*.manifest")

    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-entities", "stormkit-image", "stormkit-gpu")
    add_packages("glslang")

    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-entities")
