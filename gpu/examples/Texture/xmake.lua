target("texture")
    set_kind("binary")
    set_languages("cxx20", "clatest")
    add_rules("utils.nzsl2spv")
    add_rules("embed_resources")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_files("src/*.cpp")
    add_headerfiles("src/*.hpp")
    add_files("shaders/*.nzsl")

    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    add_defines("EXAMPLES_DATA_DIR=")
    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi", "stormkit-gpu", "stormkit-image")
    add_packages("nzsl")

    add_files("textures/texture.png", { rule = "embed_resources" })

    add_includedirs("$(buildir)/include")

    set_group("examples/stormkit-gpu")


