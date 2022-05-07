dependencies = [core_dep]

#ide:editable-filelist
headers = files([
    'include/storm/log/Fwd.hpp',
    'include/storm/log/LogHandler.hpp',
    'include/storm/log/Severity.hpp',
    'include/storm/log/Logger.hpp',
    'include/storm/log/Module.hpp',
    'include/storm/log/ConsoleLogger.hpp',
    'include/storm/log/FileLogger.hpp',
    'include/storm/log/LogColorizer.hpp',

    'include/storm/log/LogHandler.inl',
    'include/storm/log/Logger.inl',
])

#ide:editable-filelist
sources = files([
    'src/LogHandler.cpp',
    'src/Logger.cpp',
    'src/ConsoleLogger.cpp',
    'src/FileLogger.cpp',
    'src/LogColorizer.cpp',
])

log_inc = ['include/']

if lib_type == 'shared'
    log_lib_name = 'StormKit-log' + suffix
    log_lib = shared_library(
        log_lib_name,
        sources,
        extra_files: headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: log_inc,
        cpp_args: ['-DSTORMKIT_BUILD'],
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    log_lib_name = 'StormKit-log-static' + suffix
    log_lib = static_library(
        log_lib_name,
        sources,
        extra_files: headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: log_inc,
        cpp_args: ['-DSTORMKIT_BUILD'],
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    log_copy_target = custom_target(
        'log_copy_target',
        input: log_lib,
        output: 'log-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

log_dep = declare_dependency(
    link_with: log_lib,
    include_directories: log_inc,
    dependencies: dependencies,
    compile_args: cpp_arguments,
)

install_headers(headers, subdir : 'storm/log')
