glm_dep = dependency('glm')
gsl_dep = dependency('Microsoft.GSL', method: 'cmake', modules: ['Microsoft.GSL::GSL'])

dependencies = [glm_dep, gsl_dep]

#if not is_windows
    fmt_dep = dependency('fmt')
    dependencies += [fmt_dep]
#endif

if is_macos
    range_v3_dep = dependency('range-v3')
    dependencies += [range_v3_dep]
endif

#ide:editable-filelist
headers = files([
    'include/storm/core/Fwd.hpp',
    'include/storm/core/App.hpp',
    'include/storm/core/Assert.hpp',
    'include/storm/core/Concepts.hpp',
    'include/storm/core/Configure.hpp',
    'include/storm/core/Flags.hpp',
    'include/storm/core/Format.hpp',
    'include/storm/core/Hash.hpp',
    'include/storm/core/Memory.hpp',
    'include/storm/core/NamedType.hpp',
    'include/storm/core/NonCopyable.hpp',
    'include/storm/core/NonDefaultInstanciable.hpp',
    'include/storm/core/NonMovable.hpp',
    'include/storm/core/Math.hpp',
    'include/storm/core/Numerics.hpp',
    'include/storm/core/Platform.hpp',
    'include/storm/core/Singleton.hpp',
    'include/storm/core/Strings.hpp',
    'include/storm/core/Pimpl.hpp',
    'include/storm/core/Timer.hpp',
    'include/storm/core/Types.hpp',
    'include/storm/core/TypeTraits.hpp',
    'include/storm/core/Observer.hpp',
    'include/storm/core/Ranges.hpp',
    'include/storm/core/ResourcesPool.hpp',
    'include/storm/core/State.hpp',
    'include/storm/core/StateManager.hpp',
    'include/storm/core/Subject.hpp',
    'include/storm/core/Tree.hpp',
    'include/storm/core/TreeNode.hpp',
    'include/storm/core/RingBuffer.hpp',
    'include/storm/core/Span.hpp',
    'include/storm/core/Color.hpp',
    'include/storm/core/HashMap.hpp',
    'include/storm/core/ConstexprMap.hpp',

    'include/storm/core/Color.inl',
    'include/storm/core/Math.inl',
    'include/storm/core/Memory.inl',
    'include/storm/core/Numerics.inl',
    'include/storm/core/Pimpl.inl',
    'include/storm/core/Timer.inl',
    'include/storm/core/StateManager.inl',
    'include/storm/core/Subject.inl',
    'include/storm/core/Tree.inl',
    'include/storm/core/TreeNode.inl',
    'include/storm/core/RingBuffer.inl',
    'include/storm/core/ResourcesPool.inl',
])

#ide:editable-filelist
private_headers = files([
   'include/storm/core/private/Flags.hpp',
   'include/storm/core/private/robin_hood.h',
])

#ide:editable-filelist
sources = files([
    'src/State.cpp',
    'src/StateManager.cpp',
    'src/TreeNode.cpp',
    'src/Math.cpp',
])

conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('major_version', major_version)
conf_data.set('minor_version', minor_version)
conf_data.set('patch_version', patch_version)
conf_data.set('milestone', milestone)
conf_data.set('build_type', build_type)
conf_data.set('git_commit_hash', git_commit_hash)
conf_data.set('git_branch', git_branch)
conf_data.set10('STORMKIT_ENABLE_XCB', enable_window_xcb)
conf_data.set10('STORMKIT_ENABLE_WAYLAND', enable_window_wayland)
conf_data.set10('STORMKIT_ENABLE_WSI', enable_window)

configure_file(input : 'include/storm/core/Configure.hpp.in',
             output : 'Configure.hpp',
             configuration : conf_data)

core_inc = include_directories(['include/', '.'])

headers += files(meson.current_build_dir() / 'Configure.hpp')

if lib_type == 'shared'
    core_lib_name = 'StormKit-core' + suffix
    core_lib = shared_library(
        core_lib_name,
        sources,
        extra_files: headers + private_headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: core_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    core_lib_name = 'StormKit-core-static' + suffix
    core_lib = static_library(
        core_lib_name,
        sources,
        extra_files: headers + private_headers,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: core_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif

if copy_to_bindir
    core_copy_target = custom_target(
        'core_copy_target',
        input: core_lib,
        output: 'core-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
)
endif

core_dep = declare_dependency(
    link_with: core_lib, 
    include_directories: core_inc,
    dependencies: dependencies,
    compile_args: cpp_arguments,
)

install_headers(headers, subdir : 'storm/core')
install_headers(private_headers, subdir : 'storm/core/private')
