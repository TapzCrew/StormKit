rule("utils.nzsl2spv")
    set_extensions(".nzslb", ".nzsl")

    before_buildcmd_file(function (target, batchcmds, sourcefile_nzsl, opt)
        import("lib.detect.find_tool")

        -- get nzslc
        -- local nzslc = find_tool("nzslc")
        -- assert(nzslc, "nzslc not found!")

        -- nzsl to spv
        local outputdir = target:extraconf("rules", "utils.nzsl2spv", "outputdir") or path.join(target:autogendir(), "rules", "utils", "nzsl2spv")

        batchcmds:show_progress(opt.progress, "${color.build.object}generating.nzsl2spv %s", sourcefile_nzsl)
        batchcmds:mkdir(outputdir)

        batchcmds:vrunv("nzslc", { "--compile=spv", "--optimize", "--spv-version", "110", "--header-file", "-i", sourcefile_nzsl, "-o", outputdir})

        local outputfile = path.join(outputdir, format("%s.h", path.filename(sourcefile_nzsl)))

        -- add deps
        batchcmds:add_depfiles(sourcefile_nzsl)
        batchcmds:set_depmtime(os.mtime(outputfile))
        batchcmds:set_depcache(target:dependfile(outputfile))

        target:add("includedirs", outputdir)
    end)
