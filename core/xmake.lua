target("stormkit-core")
    on_load(function(target)
        local output, errors = os.iorunv("git",
                                         { "rev-parse", "--abbrev-ref", "HEAD" })

        if not errors == "" then
            print("Failed to get git hash and branch, reason: ", errors, output)
            target:set("configvar", "STORMKIT_GIT_BRANCH", " ")
            target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", " ")
            return
        end

        target:set("configvar", "STORMKIT_GIT_BRANCH", output:trim())
        output, errors = os.iorunv("git",
                                   { "rev-parse", "--verify", "HEAD" })

        target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", output:trim())
    end)

    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.inl)")
    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include(stormkit/**.h)")

    add_files("src/*.cpp")

    if is_plat("linux") then       add_files("src/posix/*.cpp")
    elseif is_plat("windows") then add_files("src/win32/*.cpp") end

    add_headerfiles("include/(stormkit/**.mpp)")
    add_headerfiles("src/**.mpp")

    add_packages("glm", "robin-hood-hashing", "frozen", "backward-cpp", { public = true })

    if not is_plat("windows") then
        add_packages("fmt", { public = true })
        add_links("pthread", "m", "dl")
    end

    add_includedirs("include", { public = true })
    add_includedirs("$(buildir)/include")

    set_configdir("$(buildir)/include/")
    add_configfiles("include/(stormkit/**.mpp.in)")

    set_group("libraries")

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include"), path.join(target:installdir(), "include"))
        os.rm(path.join(target:installdir(), "include/stormkit/core/Configure.mpp.in"))
        os.cp("$(buildir)/include/stormkit/core/Configure.mpp", path.join(target:installdir(), "include/stormkit/core/Configure.mpp"))
    end)
