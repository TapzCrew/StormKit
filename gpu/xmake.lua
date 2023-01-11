target("stormkit-gpu")
    set_kind("$(kind)")
    set_languages("cxxlatest", "clatest")

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator", { public = true })
    add_deps("stormkit-core", "stormkit-image", "stormkit-wsi", { public = true })
    add_deps("stormkit-log")

    add_defines("VK_NO_PROTOTYPES", { public = true })
    add_defines("VMA_DYNAMIC_VULKAN_FUNCTIONS=0", { public = true })
    add_defines("VMA_STATIC_VULKAN_FUNCTIONS=0", { public = true })

    add_defines("STORMKIT_BUILD")
    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        add_defines("STORMKIT_ASSERT=1")
        set_suffixname("-d")
    else
        add_defines("STORMKIT_ASSERT=0")
    end

    if not has_config("use_modules") then
        add_defines("STORMKIT_NO_MODULES") -- for syntax highlighting
    else
        add_defines("STORMKIT_BUILD_MODULES")
    end

    if is_plat("windows") then
       add_defines("VK_USE_PLATFORM_WIN32_KHR", { public = true })
    else

    end

    add_headerfiles("include/(stormkit/**.hpp)")
    if not has_config("use_modules") and has_config("enable_pch") then
        set_pcxxheader("include/stormkit/Gpu.hpp")
    end
    add_moduleorheaderfiles("modules/**.mpp")
    add_moduleorheaderfiles("src/**.mpp")
    add_files("src/**.cpp")

    add_includedirs("include", { public = true })
    add_includedirs("modules", { public = true })
    add_includedirs("$(buildir)/gen/include", { public = true })

    set_configdir("$(buildir)/gen/")
    add_configfiles("(**.hpp.in)")

    add_options("enable_wsi", "enable_wsi_x11", "enable_wsi_wayland")
    set_group("libraries")

    on_load(function(target)
        if not has_config("use_modules") or not has_config("use_cpp23_msvc_import") then
            target:data_set("c++.msvc.enable_std_import", false)
        end
    end)

    before_install(function(target)
        target:set("headerfiles")
    end)

    on_install(function(target)
        os.cp(path.join(target:scriptdir(), "include/*"), path.join(target:installdir(), "include"))
    end)

if has_config("enable_examples") and has_config("enable_wsi") then
    includes("examples/**/xmake.lua")
end
