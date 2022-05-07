compiler = meson.get_compiler('cpp')

dl_dep = compiler.find_library('dl', required: false)

dependencies = [core_dep, dl_dep]

#ide:editable-filelist
headers = files([
    'include/storm/module/Fwd.hpp',
    'include/storm/module/Module.hpp',
    'include/storm/module/AbstractModule.hpp',
])

#ide:editable-filelist
inls = files([
    'include/storm/module/Module.inl',
    'include/storm/module/AbstractModule.inl',
])

#ide:editable-filelist
sources = files([
    'src/Module.cpp',
    'src/AbstractModule.cpp',
])

if is_windows
    sources += files([
        'src/win32/ModuleImpl.hpp',
        'src/win32/ModuleImpl.cpp',
    ])
else
    sources += files([
        'src/posix/ModuleImpl.hpp',
        'src/posix/ModuleImpl.cpp',
    ])
endif

module_inc = ['include/']

if lib_type == 'shared'
    module_lib_name = 'StormKit-module' + suffix
    module_lib = shared_library(
        module_lib_name,
        sources + headers,
        extra_files: headers + inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: module_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    module_lib_name = 'StormKit-module-static' + suffix
    module_lib = static_library(
        module_lib_name,
        sources + headers,
        extra_files: headers + inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: module_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    module_copy_target = custom_target(
        'module_copy_target',
        input: module_lib,
        output: 'module-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

module_dep = declare_dependency(
    link_with: module_lib,
    include_directories: module_inc,
    dependencies: dependencies,
    compile_args: cpp_arguments,
)

install_headers(headers + inls, subdir : 'storm/module')
