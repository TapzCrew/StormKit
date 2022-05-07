target("consolelogger")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/main.cpp")

    add_deps("stormkit-core", "stormkit-main", "stormkit-log")

    add_options("enable_cxx20_modules")

    set_group("examples/stormkit-log")
