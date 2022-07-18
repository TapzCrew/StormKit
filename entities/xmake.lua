target("stormkit-entities")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("include/(stormkit/**.inl)")
    add_files("src/**.cpp")

    add_headerfiles("include/(stormkit/**.hpp)")
    add_headerfiles("src/**.hpp")

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

if has_config("enable_examples") then
    includes("examples/**/xmake.lua")
end

