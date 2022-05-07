target("polling")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/main.cpp")
    add_files("win32/*.manifest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi")

    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-wsi")


