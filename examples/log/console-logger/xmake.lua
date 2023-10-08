target("console-logger")
do
	set_kind("binary")
	set_languages("cxxlatest", "clatest")
	add_deps("stormkit-core", "stormkit-main", "stormkit-log")

	if is_mode("debug") then
		add_defines("STORMKIT_BUILD_DEBUG")
		add_defines("STORMKIT_ASSERT=1")
		set_suffixname("-d")
	else
		add_defines("STORMKIT_ASSERT=0")
	end

	add_files("src/main.cpp")

	add_ldflags("-Wl,--as-needed")
	add_shflags("-Wl,--as-needed")

	if has_config("mold") then
		add_ldflags("-Wl,-fuse-ld=mold")
		add_shflags("-Wl,-fuse-ld=mold")
	end

	set_group("examples/stormkit-log")
end
