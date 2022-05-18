target("stormkit-image")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_files("src/**.cpp")

    add_headerfiles("include/(stormkit/**.mpp)")
    add_headerfiles("src/**.mpp")

    add_packages("gli", "libpng", "libjpeg")
    add_deps("stormkit-core", { public = true })
    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include")

    set_group("libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

