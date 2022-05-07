target("stormkit-core")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("include/**.inl")
    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.h")

    add_files("src/*.cpp")

    if is_plat("linux") then       add_files("src/posix/*.cpp")
    elseif is_plat("windows") then add_files("src/win32/DynamicLoader.cpp") end

    if has_config("enable_cxx20_modules") then
        add_files("include/**.mpp")
        add_files("src/**.mpp")
    else
        add_headerfiles("include/**.mpp")
        add_headerfiles("src/**.mpp")
    end

    add_packages("glm", "robin-hood-hashing", "mapbox_eternal", "backward-cpp", { public = true })

    if not is_plat("windows") then
        add_packages("fmt", { public = true })
        add_links("pthread", "m", "dl")
    end

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include", { public = true })

    set_configdir("$(buildir)/include/stormkit/core/")
    set_configvar("STORMKIT_GIT_COMMIT_HASH", " ")
    set_configvar("STORMKIT_GIT_BRANCH", " ")
    add_configfiles("include/stormkit/core/*.mpp.in")

    add_options("enable_cxx20_modules")

    set_group("libraries")

