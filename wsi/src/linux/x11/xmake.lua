target("stormkit-wsi-linux-x11")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("*.inl")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

    add_packages("libxcb",
                 "xcb-util-keysyms",
                 "xcb-util",
                 "xcb-util-wm",
                 "xcb-util-errors")

    add_includedirs("../../../include/")
    add_deps("stormkit-core", "stormkit-log")

    add_cxxflags("-fPIC")

    set_group("private-libraries")
