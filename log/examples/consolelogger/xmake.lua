target("consolelogger")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/main.cpp")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_deps("stormkit-core", "stormkit-log")

    add_includedirs("$(buildir)/include")

    set_group("examples/stormkit-log")
