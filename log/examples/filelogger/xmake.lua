target("filelogger")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/main.cpp")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-log")

    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-log")
