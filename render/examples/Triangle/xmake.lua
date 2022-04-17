add_requires("glslang", {configs = {binaryonly = true}})

target("triangle")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.glsl2spv", {bin2c = true})

    add_files("src/*.cpp")
    add_headerfiles("src/*.mpp")
    add_files("shaders/*.vert")
    add_files("shaders/*.frag")

    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi", "stormkit-render")
    add_packages("glslang")

    add_options("enable_cxx20_modules")

add_cxxflags("-ftime-report")
add_cxxflags("-v")

    set_group("examples/stormkit-render")


