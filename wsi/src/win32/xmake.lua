target("stormkit-wsi-win32")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("*.inl")
    add_files("*.cpp")

    add_headerfiles("*.mpp")

    add_includedirs("../../include/")
    add_deps("stormkit-core", "stormkit-log", { public = true })
    add_defines("NOMINMAX", "WIN32_LEAN_AND_MEAN", { public = true })
    add_syslinks("user32", "shcore")

    set_group("private-libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)
