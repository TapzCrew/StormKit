-- Turns resources into includables headers
rule("stormkit.utils.resource2cpp")
    on_load(function(target)
        local outputdir = target:extraconf("rules", "stormkit.utils.resource2cpp", "outputdir") or path.join(target:autogendir(), "rules", "stormkit", "utils", "resource2cpp")

        if not os.isdir(outputdir) then
            os.mkdir(outputdir)
        end
    
        target:data_set("stormkit.utils.resource2cpp.outputdir", outputdir)

        target:add("includedirs", outputdir)
    end)

	before_build(function (target, opt)
		import("core.base.option")
		if xmake.version():ge("2.5.9") then
			import("utils.progress")
		elseif not import("utils.progress", { try = true }) then
			import("private.utils.progress")
		end

		local function GenerateEmbedHeader(filepath, targetpath)
			local bufferSize = 1024 * 1024

			progress.show(opt.progress, "${color.build.object}generating.resource2cpp %s", filepath)

			local resource = assert(io.open(filepath, "rb"))
			local targetFile = assert(io.open(targetpath, "w+"))

			local resourceSize = resource:size()

			local remainingSize = resourceSize
			local headerSize = 0

			while remainingSize > 0 do
				local readSize = math.min(remainingSize, bufferSize)
				local data = resource:read(readSize)
				remainingSize = remainingSize - readSize

				local headerContentTable = {}
				table.insert(headerContentTable, string.format("std::byte { %d}", data:byte(1)))
				for i = 2, #data do
					table.insert(headerContentTable, string.format(", std::byte { %d }", data:byte(i)))
				end
				local content = table.concat(headerContentTable)

				headerSize = headerSize + #content

				targetFile:write(content)
			end

			resource:close()
			targetFile:close()
		end

        local outputdir = target:data("stormkit.utils.resource2cpp.outputdir")
		for _, sourcebatch in pairs(target:sourcebatches()) do
			if sourcebatch.rulename == "stormkit.utils.resource2cpp" then
				for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
					local targetpath = path.join(outputdir, path.filename(sourcefile) .. ".hpp")
					if option.get("rebuild") or os.mtime(sourcefile) >= os.mtime(targetpath) then
						GenerateEmbedHeader(sourcefile, targetpath)
					end

				end
			end
		end
	end)