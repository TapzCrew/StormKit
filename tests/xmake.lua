local enabled_tests = {}
for name, _ in pairs(modules) do
	if name == "core" or has_config("enable_" .. name) then
		target(name .. "-tests")
      set_group("tests")
      set_kind("binary")
      set_languages("cxxlatest", "clatest")

      add_files("src/main.cpp", path.join("src", name, "**.cpp"), "src/Test.mpp", "src/Test.cpp")

      add_deps("stormkit-" .. name)
		target_end()

		table.append(enabled_tests, name .. "-tests")
	end
end

target("tests")
  set_group("tests")
  set_kind("phony")
  add_deps(table.unwrap(enabled_tests))

  on_run(function(target)
    for _, subtarget in ipairs(target:deps()) do
      subtarget:run()
    end
  end)
