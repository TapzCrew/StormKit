target("stormkit-main")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/**.hpp")

    add_deps("stormkit-core", { public = true })

    if is_plat("windows") then      add_files("src/win32/*.cpp")
    elseif is_plat("linux") then    add_files("src/linux/*.cpp")
    elseif is_plat("iphoneos") then add_files("src/ios/*.cpp")
    elseif is_plat("macosx") then   add_files("src/macos/*.cpp")
    elseif is_plat("android") then  add_files("src/android/*.cpp") end

    add_includedirs("include", { public = true })

    add_options("enable_cxx20_modules")

    set_group("libraries")

