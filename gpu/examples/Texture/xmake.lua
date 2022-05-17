add_requires("glslang", {configs = {binaryonly = true}})

target("texture")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_rules("utils.glsl2spv", {bin2c = true})
    add_rules("embed_resources")

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

    add_defines("EXAMPLES_DATA_DIR=")
    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi", "stormkit-gpu", "stormkit-image")
    add_packages("glslang")

    add_files("textures/texture.png", { rule = "embed_resources" })

    add_includedirs("$(buildir)/include")
    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-gpu")


