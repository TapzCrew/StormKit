add_requires("glslang", {configs = {binaryonly = true}})

target("texture")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.glsl2spv", {bin2c = true})

    add_files("src/*.cpp")
    add_headerfiles("src/*.mpp")
    add_files("shaders/*.vert")
    add_files("shaders/*.frag")
    add_files("win32/*.manifest")

    add_defines("EXAMPLES_DATA_DIR=")
    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi", "stormkit-gpu", "stormkit-image")
    add_packages("glslang")

    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-gpu")


