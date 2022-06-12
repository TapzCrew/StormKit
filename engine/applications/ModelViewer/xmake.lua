target("model_viewer")
    set_languages("cxxlatest", "clatest")

    add_rules("qt.quickapp")
    add_rules("utils.nzsl2spv")

    if is_mode("debug") then
        set_suffixname("-d")
    end

    add_files("src/*.cpp")
    add_files("src/*.hpp")
    add_files("*.qrc")
    add_headerfiles("src/*.mpp")

    add_deps("stormkit-engine")
    add_frameworks("QtCore",
                   "QtGui",
                   "QtQml",
                   "QtQuick",
                   "QtQuickControls2")
    add_packages("nzsl")

    add_includedirs("$(buildir)/include")

    set_group("applications/stormkit-engine")

