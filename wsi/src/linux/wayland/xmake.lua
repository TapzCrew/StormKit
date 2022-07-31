 target("stormkit-wsi-linux-wayland")
    set_kind("static")
    set_languages("cxxlatest", "clatest")

    add_headerfiles("*.inl")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

    add_packages("wayland", "wayland-protocols")

    add_includedirs("../../../include/")
    add_includedirs("$(buildir)/wayland_protocols", {public = true})
    add_deps("stormkit-core", "stormkit-log")

    add_cxxflags("-fPIC")

    set_group("private-libraries")

    after_load(function(target, opt)
        import("utils.progress")

        local wayland_install_dir           = path.absolute("..", target:pkg("wayland"):get("linkdirs"))
        local wayland_protocols_install_dir = path.absolute("..", target:pkg("wayland-protocols"):get("linkdirs"))

        local wayland_protocols_dir = path.absolute("share/wayland-protocols/", wayland_protocols_install_dir)
        local wayland_protocols_bin = path.absolute("bin/wayland-scanner", wayland_install_dir)

        local root_build_dir = path.absolute(target:configdir())

        local output_dir = path.absolute("wayland_protocols", root_build_dir)

        local protocols = { "stable/xdg-shell/xdg-shell.xml",
                            "unstable/xdg-decoration/xdg-decoration-unstable-v1.xml",
                            "unstable/pointer-constraints/pointer-constraints-unstable-v1.xml",
                            "unstable/relative-pointer/relative-pointer-unstable-v1.xml" }

        local function generate_protocol_header(name)
            -- progress.show(opt.progress, "${color.build.object}generating sources for %s", name)
            local filepath = path.absolute(name, wayland_protocols_dir)

            local output_header = path.absolute(path.basename(name) .. ".h", output_dir)
            local output_source = path.absolute(path.basename(name) .. ".c", output_dir)

            if not os.exists(output_header) then
                os.execv(wayland_protocols_bin, { "client-header", filepath, output_header})
            end

            if not os.exists(output_source) then
                os.execv(wayland_protocols_bin, { "private-code", filepath, output_source})
            end
        end

        os.mkdir(output_dir)
        for _, protocol in pairs(protocols) do
            generate_protocol_header(protocol)
        end

        target:add("files", path.absolute("*.c", output_dir))
   end)
