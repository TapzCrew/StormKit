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

