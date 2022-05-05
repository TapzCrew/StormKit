target("stormkit-wsi-win32")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("*.inl")
    add_files("*.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("*.mpp")
    else
        add_headerfiles("*.mpp")
    end


    add_includedirs("../../include/")
    add_deps("stormkit-core", "stormkit-log", { public = true })
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", { public = true })
    add_syslinks("user32", "shcore")

    add_options("enable_cxx20_modules")
    set_group("private-libraries")

