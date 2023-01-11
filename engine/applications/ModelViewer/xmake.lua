includes("xmake/**.lua")

target("model_viewer")
    add_rules("qt.quickapp")
    set_languages("cxxlatest", "clatest")
    add_rules("stormkit.utils.nzsl2spv")
    add_rules("stormkit.utils.moc2modules")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_headerfiles("src/*.hpp")
    add_files("src/*.cpp")
    add_files("src/*.mpp")
    add_files("*.qrc")

    if is_plat("windows") then
        add_files("win32/*.rc")
    end

    add_packages("nzsl")
    add_deps("stormkit-core", "stormkit-engine")

    add_frameworks("QtCore",
                   "QtGui",
                   "QtQml",
                   "QtQuick",
                   "QtQuickControls2")

    add_includedirs("$(buildir)/include")

    set_group("applications/stormkit-engine")
