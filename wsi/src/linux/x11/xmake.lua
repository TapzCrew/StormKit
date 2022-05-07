target("stormkit-wsi-linux-x11")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("**.inl")
    add_files("**.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("**.mpp")
    else
        add_headerfiles("**.mpp")
    end

    add_packages("libxcb",
                 "xcb-util-keysyms",
                 "xcb-util",
                 "xcb-util-wm",
                 "xcb-util-errors")

    add_includedirs("../../../include/")
    add_deps("stormkit-core", "stormkit-log")

    add_cxxflags("-fPIC")

    add_options("enable_cxx20_modules")
    set_group("private-libraries")
