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
      add_files("$(buildir)/.gens/modules/stormkit/Core/Configuration.mpp", { always_added = true, public = true })
      add_cxflags("clang::-Wno-language-extension-token")

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
    public_packages = { "frozen" },
    modulename = "Log",
    public_deps = { "stormkit-core" },
    has_headers = true,
  },
  entities = {
    modulename = "Entities",
    public_deps = { "stormkit-core" },
  },
  image = {
    public_packages = { "frozen" },
    packages = { "gli", "libpng", "libjpeg-turbo" },
    modulename = "Image",
    public_deps = { "stormkit-core" },
  },
  main = {
    modulename = "Main",
    has_headers = true,
    deps = { "stormkit-core" },
    custom = function()
        add_cxflags("-Wno-main")
        set_strip("debug")
    end
  },
  wsi = {
    modulename = "Wsi",
    public_deps = { "stormkit-core" },
    deps = { "stormkit-log" },
    packages = is_plat("linux") and {
      "libxkbcommon",
      "libxkbcommon-x11",
      "libxcb",
      "xcb-util-keysyms",
      "xcb-util",
      "xcb-util-wm",
      "xcb-util-errors",
      "wayland",
      "wayland-protocols",
    } or nil,
    frameworks = is_plat("macosx") and { "CoreFoundation", "Foundation", "AppKit", "Metal", "IOKit", "QuartzCore" }
        or nil,
    custom = function()
      if is_plat("linux") then
        add_rules("wayland.protocols")

        on_load(function(target)
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
      if is_plat("mingw") then
        add_syslinks("user32", "shell32")
        add_ldflags("-Wl,-mwindows", { public = true })
      elseif is_plat("windows") then
        add_syslinks("user32", "shell32")
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
    public_packages = { "vulkan-headers", "vulkan-memory-allocator", "vulkan-memory-allocator-hpp" },
    public_deps = { "stormkit-core", "stormkit-log", "stormkit-wsi", "stormkit-image" },
    packages = is_plat("linux") and {
      "libxcb",
      "wayland",
    } or nil,
    defines = {
      "VK_NO_PROTOTYPES",
      "VMA_DYNAMIC_VULKAN_FUNCTIONS=1",
      "VMA_STATIC_VULKAN_FUNCTIONS=0",
      "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
      "VULKAN_HPP_NO_STRUCT_CONSTRUCTORS",
      "VULKAN_HPP_NO_UNION_CONSTRUCTORS",
      "VULKAN_HPP_STORAGE_SHARED",
      -- "VULKAN_HPP_NO_EXCEPTIONS", uncomment when vk::raii is supported without exceptions
    },
    custom = function()
      if is_plat("linux") then
        add_defines("VK_USE_PLATFORM_XCB_KHR")
        add_defines("VK_USE_PLATFORM_WAYLAND_KHR")
      elseif is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
      end
    end,
  },
}

package("vulkan-memory-allocator", function()
  set_kind("library", { headeronly = true })
  set_homepage("https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/")
  set_description("Easy to integrate Vulkan memory allocation library.")
  set_license("MIT")

  add_urls(
    "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/$(version).tar.gz",
    "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
  )

  on_install("windows", "linux", "mingw", "macosx", "iphoneos", "android", function(package)
    os.cp("include/vk_mem_alloc.h", package:installdir("include"))
  end)
end)

package("vulkan-memory-allocator-hpp", function()
  set_kind("library", { headeronly = true })
  set_homepage("https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/")
  set_description("C++ bindings for VulkanMemoryAllocator.")
  set_license("CC0")

  add_urls(
    "https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/archive/refs/tags/$(version).tar.gz",
    "https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git"
  )

  on_install("windows|x86", "windows|x64", "linux", "macosx", "mingw", "android", "iphoneos", function(package)
    os.cp("include", package:installdir())
  end)
end)

package("frozen", function()
  set_homepage("https://github.com/serge-sans-paille/frozen")
  set_description("A header-only, constexpr alternative to gperf for C++14 users")
  set_license("Apache-2.0")

  set_urls("https://github.com/Arthapz/frozen.git")
  -- set_sourcedir("../frozen")

  on_install(function(package)
    import("package.tools.xmake").install(package, { enable_module = true, enable_std_import = true, enable_tests = false, enable_benchmark = false })
  end)
end)

local allowedmodes = {
  "debug",
  "release",
  "releasedbg",
  "check",
  "coverage",
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

option("sanitizers", { default = false, category = "root menu/build" })
option("mold", { default = false, category = "root menu/build" })

---------------------------- module options ----------------------------
option("enable_log", { default = true, category = "root menu/modules" })
option("enable_entities", { default = true, category = "root menu/modules" })
option("enable_image", { default = true, category = "root menu/modules" })
option("enable_main", { default = true, category = "root menu/modules" })
option("enable_wsi", { default = true, category = "root menu/modules" })
option(
  "enable_gpu",
  { default = true, category = "root menu/modules", deps = { "enable_log", "enable_image", "enable_wsi" } }
)
option("enable_engine", {
  default = true,
  category = "root menu/modules",
  deps = { "enable_log", "enable-entities", "enable_image", "enable_wsi", "enable_gpu" },
})

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
  { tools = { "cl", "clang_cl" } }
)

add_cxflags(
  "/wd4251", -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
  "/wd4297",
  "/wd5063",
  "/wd5260",
  "/wd5050",
  "/wd4005",
  "/wd4611", -- Disable setjmp warning
  { tools = { "cl", "clang_cl" } }
)

add_cxflags("-fstrict-aliasing", "-Wstrict-aliasing", { tools = { "clang", "gcc" } })
add_mxflags("-fstrict-aliasing", "-Wstrict-aliasing", { tools = { "clang", "gcc" } })

add_cxflags("clang::-Wno-missing-field-initializers", "clang::-Wno-include-angled-in-module-purview",
  "clang::-Wno-unknown-attributes", "-Wno-deprecated-declarations")
add_mxflags("clang::-Wno-missing-field-initializers")

set_symbols("hidden")
set_optimize("fastest")
if is_mode("debug") then
  set_symbols("debug", "hidden")
  add_defines("_GLIBCXX_DEBUG")
  add_cxflags("-ggdb3", { tools = { "clang", "gcc" } })
  add_mxflags("-ggdb3", { tools = { "clang", "gcc" } })
elseif is_mode("releasedbg") then
  set_optimize("fast")
  set_symbols("debug", "hidden")
  add_cxflags("-fno-omit-frame-pointer", { tools = { "clang", "gcc" } })
  add_mxflags("-fno-omit-frame-pointer", { tools = { "clang", "gcc" } })
  add_cxflags("-ggdb3", { tools = { "clang", "gcc" } })
  add_mxflags("-ggdb3", { tools = { "clang", "gcc" } })
end

set_fpmodels("fast")
add_vectorexts("fma")
add_vectorexts("neon")
add_vectorexts("avx", "avx2")
add_vectorexts("sse", "sse2", "sse3", "ssse3", "sse4.2")

set_warnings("all", "pedantic", "extra")

add_cxxflags("clang::-Wno-experimental-header-units")
add_cxxflags("clang::-fcolor-diagnostics", "gcc::-fdiagnostics-color=always")

if is_plat("windows") then
  add_defines("_CRT_SECURE_NO_WARNINGS")
  add_defines("WIN32_LEAN_AND_MEAN")
  add_defines("__SPECSTRINGS_STRICT_LEVEL=0")
  add_defines("NOMINMAX")
  add_cxflags("-fansi-escape-codes")
end

add_cxxflags("clang::-fexperimental-library", { force = true })
add_ldflags("clang::-fexperimental-library", { force = true })
add_shflags("clang::-fexperimental-library", { force = true })
add_mxxflags("clang::-fexperimental-library", { force = true })

if get_config("sanitizers") then
  set_policy("build.sanitizer.address", true)
  set_policy("build.sanitizer.undefined", true)
end

if has_config("enable_gpu") then
  add_requires("vulkan-headers main", { system = false })
  add_requires("vulkan-memory-allocator master", { system = false })
  add_requires("vulkan-memory-allocator-hpp master", { system = false })
end

-- add_defines("FROZEN_DONT_INCLUDE_STL", "ANKERL_UNORDERED_DENSE_USE_STD_IMPORT")
add_requireconfs("*", { configs = { modules = true } })

---------------------------- targets ----------------------------
for name, module in pairs(modules) do
  add_requires(table.join(module.packages or {}, module.public_packages or {}))

  local modulename = module.modulename

  if name == "core" or name == "main" or get_config("enable_" .. name) then
    target("stormkit-" .. name, function()
      set_group("libraries")

      if module.custom then
        module.custom()
      end

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
        add_files(module_path .. ".mpp", { public = true })
      end

      if os.files(module_path) then
        for _, file in ipairs(os.files(path.join(module_path, "**.mpp"))) do
          add_files(file, { public = true })
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
      if is_mode("release") then
          -- set_policy("build.optimization.lto", true)
      end
    end)
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
