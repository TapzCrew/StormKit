add_requires("llfio", {configs = {cpp20 = true}})

target("ifcreader")
    set_kind("binary")
    set_languages("cxxlatest", "clatest")
    add_files("src/**.cpp")
    add_files("src/**.mpp")
    add_packages("llfio")

    add_deps("stormkit-core", "stormkit-main")

    set_group("tools")
