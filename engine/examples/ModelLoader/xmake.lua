add_requires("glslang", {configs = {binaryonly = true}})

target("model_loader")
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

    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-engine", "stormkit-log")
    add_packages("glslang")

    add_includedirs("$(buildir)/include")
    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-engine")


