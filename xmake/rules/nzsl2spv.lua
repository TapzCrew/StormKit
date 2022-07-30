rule("stormkit.utils.nzsl2spv")
    set_extensions(".nzslb", ".nzsl")

    on_load(function(target)
        local outputdir = target:extraconf("rules", "stormkit.utils.nzsl2spv", "outputdir") or path.join(target:autogendir(), "rules", "stormkit", "utils", "nzsl2spv")

        if not os.isdir(outputdir) then
            os.mkdir(outputdir)
        end
    
        target:data_set("stormkit.utils.nzsl2spv.outputdir", outputdir)

        target:add("includedirs", outputdir)
    end)

    before_build(function (target, opt)
		if xmake.version():ge("2.5.9") then
			import("utils.progress")
		elseif not import("utils.progress", { try = true }) then
			import("private.utils.progress")
		end

		import("core.base.option")
        import("lib.detect.find_tool")

        local outputdir = target:data("stormkit.utils.nzsl2spv.outputdir")

        -- get nzslc
        --local nzslc = find_tool("nzslc") or { program = "nzslc" }
        local nzslc = "nzslc"
        assert(nzslc, "nzslc not found!")

        -- nzsl to spv
		for _, sourcebatch in pairs(target:sourcebatches()) do
			if sourcebatch.rulename == "stormkit.utils.nzsl2spv" then
				for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    local outputfile = path.join(outputdir, path.filename(sourcefile) .. ".h")

					if option.get("rebuild") or os.mtime(sourcefile) >= os.mtime(outputfile) then
			            progress.show(opt.progress, "${color.build.object}generating.nzsl2spv %s", sourcefile)
                        os.vrunv(nzslc, { "--compile=spv-header", "--optimize", "--spv-version", "110", "-i", sourcefile, "-o", outputdir})
                    end
                end
            end
        end
    end)
