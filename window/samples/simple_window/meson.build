dependencies = [core_dep, log_dep, window_dep, main_dep]

#ide:editable-filelist
sources = files([
    'src/main.cpp',
])

simple_window = executable(
    'stormkit_SimpleWindow',
    sources,
    dependencies: dependencies,
    build_rpath: './',
    install: true
)
destination_path = join_paths(meson.project_build_root(), 'output/bin/')

if copy_to_bindir
    simple_window_copy_target = custom_target(
        'simple_window_copy_target',
        input: simple_window,
        output: 'simple-window-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif
