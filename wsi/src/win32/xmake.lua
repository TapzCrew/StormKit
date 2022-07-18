target("stormkit-wsi-win32")
    set_kind("static")
    set_languages("cxx20", "clatest")

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        set_suffixname("-d")
    end

    add_headerfiles("*.hpp")
    add_headerfiles("*.inl")
    add_files("*.cpp")

    add_includedirs("../../include/")
    add_deps("stormkit-core", "stormkit-log", { public = true })
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", { public = true })
    add_syslinks("user32", "shcore")

    set_group("private-libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)
