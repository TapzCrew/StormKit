target("stormkit-log")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/**.hpp")
    add_headerfiles("include/**.inl")
    add_files("src/**.cpp")

    if has_config("enable_cxx20_modules") then
        add_files("include/**.mpp")
        add_files("src/**.mpp")
    else
        add_headerfiles("include/**.mpp")
        add_headerfiles("src/**.mpp")
    end

    add_deps("stormkit-core", { public = true })

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include", { public = true })

    if is_plat("windows") then
        remove_files("src/posix/*")
    else
        remove_files("src/win32/*")
    end

    add_options("enable_cxx20_modules")

    set_group("libraries")

includes("examples/**/xmake.lua")
