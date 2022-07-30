target("event_handler")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/main.cpp")

    if is_plat("windows") then
        add_files("win32/*.manifest")
    end

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_deps("stormkit-core", "stormkit-main", "stormkit-log", "stormkit-wsi")

    set_group("examples/stormkit-wsi")


