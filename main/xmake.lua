target("stormkit-main")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")

    add_deps("stormkit-core", { public = true })

    if is_plat("windows") then      add_files("src/win32/*.cpp")
    elseif is_plat("linux") then    add_files("src/linux/*.cpp")
    elseif is_plat("iphoneos") then add_files("src/ios/*.cpp")
    elseif is_plat("macosx") then   add_files("src/macos/*.mm")
    elseif is_plat("android") then  add_files("src/android/*.cpp") end

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include")

    set_group("libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)
