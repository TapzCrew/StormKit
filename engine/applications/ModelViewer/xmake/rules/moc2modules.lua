rule("stormkit.utils.moc2modules")
    set_extensions(".mpp", ".mxx", ".cppm", ".ixx")
    on_config(function (target)
        import("core.tool.compiler")

        local qt = assert(target:data("qt"), "qt not found!")
        local moc = path.join(qt.bindir, is_host("windows") and "moc.exe" or "moc")
        if not os.isexec(moc) and qt.libexecdir then
            moc = path.join(qt.libexecdir, is_host("windows") and "moc.exe" or "moc")
        end
        if not os.isexec(moc) and qt.libexecdir_host then
            moc = path.join(qt.libexecdir_host, is_host("windows") and "moc.exe" or "moc")
        end
        assert(moc and os.isexec(moc), "moc not found!")

        for _, sourcefile in ipairs(target:sourcebatches()["stormkit.utils.moc2modules"].sourcefiles) do
            local basename = path.basename(sourcefile)
            local filename_moc = "moc_" .. basename .. ".cpp"
            if sourcefile:endswith(".cpp") then
                filename_moc = basename .. ".moc"
            end
            local sourcefile_moc = target:autogenfile(path.join(path.directory(sourcefile), filename_moc))
            if not os.isdir(path.directory(sourcefile_moc)) then
                os.mkdir(path.directory(sourcefile_moc))
            end

            local objectfile = target:objectfile(sourcefile_moc)
            table.insert(target:objectfiles(), objectfile)

            local flags = {}
            table.join2(flags, compiler.map_flags("cxx", "define", target:get("defines")))
            local pathmaps = {
                {"includedirs", "includedir"},
                {"sysincludedirs", "includedir"}, 
                {"frameworkdirs", "frameworkdir"}
            }
            for _, pathmap in ipairs(pathmaps) do
                for _, item in ipairs(target:get(pathmap[1])) do
                    local pathitem = path(item, function (p)
                        local item = table.unwrap(compiler.map_flags("cxx", pathmap[2], p))
                        if item then
                            item = item:gsub("\\", "/")
                        end
                        return item
                    end)
                    table.insert(flags, pathitem)
                end
            end
            local user_flags = target:get("qt.moc.flags") or {}
            vprint("generating.qt.moc2modules %s", sourcefile)
            os.vrunv(moc, table.join(user_flags, flags, path(sourcefile), "-o", path(sourcefile_moc)))

            local mocsourcecode = io.readfile(sourcefile_moc)
            assert(mocsourcecode)

            mocsourcecode = mocsourcecode:gsub("//.-\n", "\n")
            mocsourcecode = mocsourcecode:gsub("/%*.-%*/", "")

            local mocdata = io.readfile(sourcefile)
            local modulename
            for _, line in ipairs(mocdata:split("\n", {plain=true})) do
                modulename = line:match("export%s+module%s+(.+)%s*;")
                if modulename then
                    break
                end
            end

            if not modulename and mocdata and mocdata:find("Q_PRIVATE_SLOT") or sourcefile_moc:endswith(".moc") then
                target:add("includedirs", path.directory(sourcefile_moc))

                local objectfiles = target:objectfiles()
                for idx, objectfile in ipairs(objectfiles) do
                    if objectfile == target:objectfile(sourcefile_moc) then
                        table.remove(objectfiles, idx)
                    end
                end
            else
                mocsourcecode = "module;\n" .. "\n#include <QtCore/QMap>\n#include <QtCore/QObjectData>" .. mocsourcecode
                mocsourcecode = mocsourcecode:gsub("#endif", "#endif\n\nmodule " .. modulename .. ";")


                --print(mocsourcecode)
                io.writefile(sourcefile_moc, mocsourcecode)

                target:add("files", sourcefile_moc)
            end
        end
    end)
