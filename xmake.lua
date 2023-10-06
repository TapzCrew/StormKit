modules = {
	core = {
		public_packages = { "glm", "frozen", "unordered_dense", "magic_enum" },
		modulename = "Core",
		has_headers = true,
		custom = function()
			if is_plat("windows") then
				add_packages("wil")
			end

			set_configdir("$(buildir)/.gens/modules/stormkit/Core")
			add_filegroups("generated", { rootdir = "$(buildir)/.gens/modules" })

			if not os.exists("$(buildir)/.gens/modules/stormkit/Core/Configuration.mpp") then
				add_configfiles("modules/stormkit/Core/(**.mpp.in)")
			end

			add_files("modules/stormkit/Core.mpp")
			add_files("$(buildir)/.gens/modules/stormkit/Core/Configuration.mpp", { always_added = true })

			on_config(function(target)
				local output, errors = os.iorunv("git", { "rev-parse", "--abbrev-ref", "HEAD" })

				if not errors == "" then
					print("Failed to get git hash and branch, reason: ", errors, output)
					target:set("configvar", "STORMKIT_GIT_BRANCH", " ")
					target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", " ")
					return
				end

				target:set("configvar", "STORMKIT_GIT_BRANCH", output:trim())
				output, errors = os.iorunv("git", { "rev-parse", "--verify", "HEAD" })

				target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", output:trim())
			end)
		end,
	},
	log = {
		modulename = "Log",
		public_deps = { "stormkit-core" },
		has_headers = true,
	},
	entities = {
		modulename = "Entities",
		public_deps = { "stormkit-core" },
	},
	image = {
		packages = { "gli", "libpng", "libjpeg-turbo" },
		modulename = "Image",
		public_deps = { "stormkit-core" }
	},
	main = {
		modulename = "Main",
		has_headers = true,
		deps = { "stormkit-core" },
	},
	wsi = {
		modulename = "Wsi",
		public_deps = { "stormkit-core" },
		deps = { "stormkit-log" },
		packages = is_plat("linux") and { "libxkbcommon",
            "libxkbcommon-x11",
            "libxcb",
            "xcb-util-keysyms",
            "xcb-util",
            "xcb-util-wm",
            "xcb-util-errors",
            "wayland",
            "wayland-protocols",
          } or nil,
    frameworks = is_plat("macosx") and {"CoreFoundation", "Foundation", "AppKit", "Metal", "IOKit", "QuartzCore" } or nil,
		custom = function()
			if is_plat("linux") then
				add_rules("wayland.protocols")

				on_load(function(target)
					local pkg = target:pkg("wayland-protocols")
					local wayland_protocols_dir =
						path.join(target:pkg("wayland-protocols"):installdir() or "/usr", "share", "wayland-protocols")
					assert(wayland_protocols_dir, "wayland protocols directory not found")

					local protocols = {
						path.join("stable", "xdg-shell", "xdg-shell.xml"),
						path.join("unstable", "xdg-decoration", "xdg-decoration-unstable-v1.xml"),
						path.join("unstable", "pointer-constraints", "pointer-constraints-unstable-v1.xml"),
						path.join("unstable", "relative-pointer", "relative-pointer-unstable-v1.xml"),
					}

					for _, protocol in ipairs(protocols) do
						target:add("files", path.join(wayland_protocols_dir, protocol))
					end
				end)
			end
		end,
	},
	engine = {
		modulename = "Engine",
		has_headers = true,
		public_deps = {
			"stormkit-core",
			"stormkit-log",
			"stormkit-wsi",
			"stormkit-image",
			"stormkit-entities",
			"stormkit-gpu",
		},
	},
	gpu = {
		modulename = "Gpu",
		has_headers = true,
		public_deps = { "stormkit-core", "stormkit-log", "stormkit-wsi", "stormkit-image" },
		public_packages = { "vulkan-headers", "vulkan-memory-allocator", "vulkan-memory-allocator-hpp" },
		private_packages = {
			"libxcb",
			"wayland",
		},
		defines = {
			"VK_NO_PROTOTYPES",
			"VMA_DYNAMIC_VULKAN_FUNCTIONS=1",
			"VMA_STATIC_VULKAN_FUNCTIONS=0",
			"VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
			"VULKAN_HPP_NO_STRUCT_CONSTRUCTORS",
			"VULKAN_HPP_STORAGE_SHARED",
			-- "VULKAN_HPP_NO_EXCEPTIONS", uncomment when vk::raii is supported without exceptions
		},
		custom = function()
      remove_files("src/Gpu/Execution/**.cpp")
      remove_files("modules/stormkit/Gpu/Execution.mpp")
      remove_files("modules/stormkit/Gpu/Execution/**.mpp")
			if is_plat("linux") then
				add_defines("VK_USE_PLATFORM_XCB_KHR")
				add_defines("VK_USE_PLATFORM_WAYLAND_KHR")
			elseif is_plat("windows") then
				add_defines("VK_USE_PLATFORM_WIN32_KHR")
			end
		end,
	},
}

package("vulkan-memory-allocator")
set_kind("library", { headeronly = true })
set_homepage("https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/")
set_description("Easy to integrate Vulkan memory allocation library.")
set_license("MIT")

add_urls(
	"https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/$(version).tar.gz",
	"https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
)

add_deps("vulkan-headers", { override = true, version = "main", system = false })

on_install("windows", "linux", "mingw", "macosx", "iphoneos", "android", function(package)
	os.cp("include/vk_mem_alloc.h", package:installdir("include"))
end)
package_end()

package("vulkan-memory-allocator-hpp")
set_kind("library", { headeronly = true })
set_homepage("https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/")
set_description("C++ bindings for VulkanMemoryAllocator.")
set_license("CC0")

add_urls(
	"https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/archive/refs/tags/$(version).tar.gz",
	"https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git"
)

add_deps("vulkan-memory-allocator", { override = true, version = "master", system = false })

on_install("windows|x86", "windows|x64", "linux", "macosx", "mingw", "android", "iphoneos", function(package)
	os.cp("include", package:installdir())
	if package:gitref() or package:version():ge("3.0.1") then
		package:add("deps", "vulkan-hpp >= 1.3.234")
	else
		package:add("deps", "vulkan-hpp < 1.3.234")
	end
end)
package_end()

add_requireconfs("vulkan-headers", { override = true, version = "main", system = false })
add_requireconfs("vulkan-memory-allocator", { override = true, version = "master", system = false })
add_requireconfs("vulkan-memory-allocator-hpp", { override = true, version = "master", system = false })

local allowedmodes = {
	"debug",
	"release",
	"releasedbg",
	"check",
	"coverage",
	"asan",
	"lsan",
	"ubsan",
	"profile",
	"check",
	"minsizerel",
}

add_repositories("nazara-engine-repo https://github.com/NazaraEngine/xmake-repo")

set_xmakever("2.7.4")
set_project("StormKit")

set_version("0.1.0", { build = "%Y%m%d%H%M" })

includes("xmake/**.lua")

---------------------------- global rules ----------------------------
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build", lsp = "clangd" })
add_rules(
	"mode.debug",
	"mode.release",
	"mode.releasedbg",
	"mode.check",
	"mode.coverage",
	"mode.asan",
	"mode.lsan",
	"mode.tsan",
	"mode.ubsan",
	"mode.profile",
	"mode.check",
	"mode.minsizerel"
)

if not is_plat("windows") or not is_plat("mingw") then
	add_rules("mode.valgrind")
end

---------------------------- global options ----------------------------
option("enable_examples", { default = false, category = "root menu/others" })
option("enable_applications", { default = false, category = "root menu/others" })
option("enable_tests", { default = false, category = "root menu/others" })

option("enable_entities", { default = true, category = "root menu/modules" })
option("enable_log", { default = true, category = "root menu/modules" })
option("enable_image", { default = true, category = "root menu/modules" })
option("enable_wsi", { default = true, category = "root menu/modules" })
option("enable_gpu", { default = true, category = "root menu/modules" })
option("enable_engine", { default = false, category = "root menu/modules" })

---------------------------- global config ----------------------------
set_allowedmodes(allowedmodes)
set_allowedplats("windows", "mingw", "linux", "macosx")
set_allowedarchs("windows|x64", "mingw|x86_64", "linux|x86_64", "macosx|x86_64")

-- set_runtimes(is_mode("debug") and "MDd" or "MD")
add_cxxflags(
	"/utf-8",
	"/bigobj",
	"/permissive-",
	"/Zc:wchar_t",
	"/Zc:__cplusplus",
	"/Zc:inline",
	"/Zc:lambda",
	"/Zc:preprocessor",
	"/Zc:referenceBinding",
	"/Zc:strictStrings",
	{ tools = { "msvc", "clang-cl" } }
)

add_cxflags(
	"/wd4251", -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
	"/wd4297",
	"/wd5063",
	"/wd5260",
	"/wd5050",
	"/wd4005",
	"/wd4611", -- Disable setjmp warning
	{ tools = { "msvc", "clang-cl" } }
)

add_cxflags("-fstrict-aliasing", "-Wstrict-aliasing", { tools = { "clang", "gcc" } })
add_mxflags("-fstrict-aliasing", "-Wstrict-aliasing", { tools = { "clang", "gcc" } })

add_cxxflags(
	"-Wno-missing-field-initializers",
	{ tools = { "clang" } }
)
add_mxxflags(
	"-Wno-missing-field-initializers",
	{ tools = { "clang" } }
)

if not is_plat("windows") and not is_plat("macosx") then
	add_syslinks("stdc++_libbacktrace")
end

if is_mode("release") then
	set_symbols("hidden")
	set_optimize("fastest")
else
	set_symbols("debug", "hidden")
	add_defines("_GLIBCXX_DEBUG")
	add_cxflags("-ggdb3")
	add_mxflags("-ggdb3")
end

set_fpmodels("fast")
add_vectorexts("mmx")
add_vectorexts("neon")
add_vectorexts("avx", "avx2")
add_vectorexts("sse", "sse2", "sse3", "ssse3", "sse4.2")

-- set_policy("build.optimization.lto", true)
set_warnings("all", "pedantic", "extra")

option("libc++")
set_default(false)
option_end()

option("mold")
set_default(false)
add_ldflags("-Wl,-fuse-ld=mold")
add_shflags("-Wl,-fuse-ld=mold")
option_end()

if get_config("libc++") then
	if is_plat("windows") then
		add_sysincludedirs("C:/Dev/llvm/include/c++/v1")
	end
	add_cxxflags("-stdlib=libc++", "-fexperimental-library")
	add_mxxflags("-stdlib=libc++", "-fexperimental-library")
	set_policy("build.c++.clang.fallbackscanner", true)

	target("stdmodules")
    set_kind("object")
    set_languages("c++latest")

    add_files("stdmodules/**.cppm")

    add_cxxflags("-Wno-reserved-module-identifier")
    set_policy("build.c++.clang.fallbackscanner", false)
	target_end()
end

---------------------------- targets ----------------------------
for name, module in pairs(modules) do
	if not name == "core" then
		option("enable_" .. name)
		set_default(true)
		set_category("root menu/modules")
		for _, deps in ipairs(module.deps) do
			add_deps("enable_" .. deps)
		end
		option_end()
	end

	add_requires(table.join(module.packages or {}, module.public_packages or {}))

	local modulename = module.modulename

	if name == "core" or name == "main" or has_config("enable_" .. name) then
		target("stormkit-" .. name)
		set_group("libraries")

		if name == "main" then
			set_kind("static")
		else
			set_kind("$(kind)")
		end

		set_languages("cxxlatest", "clatest")

		add_defines("STORMKIT_BUILD")
		if is_mode("debug") then
			add_defines("STORMKIT_BUILD_DEBUG")
			add_defines("STORMKIT_ASSERT=1")
			set_suffixname("-d")
		else
			add_defines("STORMKIT_ASSERT=0")
		end

		if is_kind("static") then
			add_defines("STORMKIT_STATIC", { public = true })
		end

		local src_path = path.join("src", modulename)
		local module_path = path.join("modules", "stormkit", modulename)
		local include_path = path.join("include", "stormkit", modulename)

		for _, file in ipairs(os.files(path.join(src_path, "**.mpp"))) do
			add_files(file)
		end
		for _, file in ipairs(os.files(path.join(src_path, "**.cpp"))) do
			add_files(file)
		end
		for _, file in ipairs(os.files(path.join(src_path, "**.mm"))) do
			add_files(file)
		end
		for _, file in ipairs(os.files(path.join(src_path, "**.m"))) do
			add_files(file)
		end
		for _, file in ipairs(os.files(path.join(src_path, "**.inl"))) do
			add_files(file)
		end

		if os.exists(module_path .. ".mpp") then
			add_files(module_path .. ".mpp")
		end

		if os.files(module_path) then
			for _, file in ipairs(os.files(path.join(module_path, "**.mpp"))) do
				add_files(file)
			end
			for _, file in ipairs(os.files(path.join(module_path, "**.inl"))) do
				add_headerfiles(file)
			end
		end

		for _, file in ipairs(os.files(path.join(include_path, "**.inl"))) do
			add_headerfiles(file)
		end
		for _, file in ipairs(os.files(path.join(include_path, "**.hpp"))) do
			add_headerfiles(file)
		end

		if is_plat("windows") or is_plat("mingw") then
			for _, plat in ipairs({ "posix", "linux", "darwin", "macOS", "iOS", "BSD", "Android" }) do
				remove_files(path.join(src_path, plat, "**"))
				remove_headerfiles(path.join(src_path, plat, "**"))
			end
		elseif is_plat("macosx") then
			for _, plat in ipairs({ "linux", "win32", "iOS", "BSD", "Android" }) do
				remove_files(path.join(src_path, plat, "**"))
				remove_headerfiles(path.join(src_path, plat, "**"))
			end
		elseif is_plat("ios") then
			for _, plat in ipairs({ "linux", "macOS", "win32", "BSD", "Android" }) do
				remove_files(path.join(src_path, plat, "**"))
				remove_headerfiles(path.join(src_path, plat, "**"))
			end
		elseif is_plat("android") then
			for _, plat in ipairs({ "linux", "darwin", "macOS", "iOS", "BSD", "win32" }) do
				remove_files(path.join(src_path, plat, "**"))
				remove_headerfiles(path.join(src_path, plat, "**"))
			end
		elseif is_plat("linux") then
			for _, plat in ipairs({ "win32", "darwin", "macOS", "iOS", "BSD", "Android" }) do
				remove_files(path.join(src_path, plat, "**"))
				remove_headerfiles(path.join(src_path, plat, "**"))
			end
		end

		add_includedirs("$(projectdir)/include", { public = true })

		if module.defines then
			add_defines(module.defines)
		end

		if module.cxxflags then
			add_cxxflags(module.cxxflags)
			add_mxxflags(module.cxxflags)
		end

		if module.public_deps then
			add_deps(module.public_deps, { public = true })
		end

		if module.deps then
			add_deps(module.deps, { public = is_kind("static") })
		end

		if module.public_packages then
			add_packages(module.public_packages, { public = true })
		end

		if module.packages then
			add_packages(module.packages, { public = is_kind("static") })
		end

		if module.frameworks then
			add_frameworks(module.frameworks, { public = is_kind("static") })
		end
		if module.custom then
			module.custom()
		end

		if get_config("libc++") then
			add_deps("stdmodules")
		end
		target_end()
	end
end

if has_config("enable_examples") then
	for name, _ in pairs(modules) do
		local example_dir = path.join("examples", name)
		if os.exists(example_dir) and has_config("enable_" .. name) then
			includes(path.join(example_dir, "**", "xmake.lua"))
		end
	end
end

if has_config("enable_tests") then
	includes("tests/xmake.lua")
end
