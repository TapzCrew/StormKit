dependencies = [core_dep]

#ide:editable-filelist
headers = files([
    'include/storm/entities/Fwd.hpp',
    'include/storm/entities/Component.hpp',
    'include/storm/entities/Entity.hpp',
    'include/storm/entities/EntityManager.hpp',
    'include/storm/entities/System.hpp',
    'include/storm/entities/MessageBus.hpp',

    'include/storm/entities/EntityManager.inl',
])

#ide:editable-filelist
sources = files([
    'src/EntityManager.cpp',
    'src/System.cpp',
    'src/Component.cpp',
    'src/MessageBus.cpp',
])

entities_inc = include_directories('include/')

if lib_type == 'shared'
    entities_lib_name = 'StormKit-entities' + suffix
    entities_lib = shared_library(
        entities_lib_name,
        sources,
        extra_files: headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: entities_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    entities_lib_name = 'StormKit-entities-static' + suffix
    entities_lib = static_library(
        entities_lib_name,
        sources,
        extra_files: headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: entities_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    entities_copy_target = custom_target(
        'entities_copy_target',
        input: entities_lib,
        output: 'entities-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

entities_dep = declare_dependency(
    link_with: entities_lib, 
    include_directories: entities_inc,
    dependencies: dependencies,
    compile_args: cpp_arguments,
)

install_headers(headers, subdir : 'storm/entities')
