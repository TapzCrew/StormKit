dependencies = []

#ide:editable-filelist
headers = files([
    'include/storm/main/Main.hpp',
])

#ide:editable-filelist
sources = files([])
args = {}

if is_windows
    sources += files([
        'src/win32/Main.cpp',
    ])
elif is_linux
    sources += files([
        'src/linux/Main.cpp',
    ])
elif is_macos
    add_languages('objc')
    add_languages('objcpp')
    sources += files([
        'src/macOS/Main.mm',
    ])
    #args += {'cpp_args': '-x objective-c++'}

    foundation_dep = dependency('Foundation')

    dependencies += [foundation_dep]
elif is_ios
    sources += files([
        'src/iOS/Main.mm',
    ])
    args += {'cpp_args': '-x objective-c++'}
endif

main_inc = ['include/']

args += {
    'dependencies': dependencies,
    'include_directories': main_inc
}

main_lib = static_library(
    'StormKit-main' + suffix, 
    sources,
    extra_files: headers,
    kwargs: args,
    install: true
)
destination_path = join_paths(meson.project_build_root(), 'output/lib/')


if copy_to_bindir
    main_copy_target = custom_target(
        'main_copy_target',
        input: main_lib,
        output: 'main-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

main_dep = declare_dependency(
    link_with: main_lib,
    include_directories: main_inc,
    dependencies: dependencies,
    compile_args: cpp_arguments,
)

install_headers(headers, subdir : 'storm/main')
