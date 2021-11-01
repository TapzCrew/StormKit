project(
    'StormKit',
    ['c', 'cpp'],
    version: '0.2.0',
    license: 'MIT',
    default_options : ['c_std=c17', 'default_library=static']
)

system_name = host_machine.system()

is_windows = system_name == 'windows'
is_linux = system_name == 'linux'
is_macos = system_name == 'darwin'
is_ios = system_name == 'ios'
is_android = system_name == 'android'

examples_data_dir = get_option('examples_data_dir')

lib_type = get_option('library_type')

c_arguments = []
cpp_arguments = []

lib_extension   = ''

compiler = meson.get_compiler('cpp')
major_version   = 0
minor_version   = 2
patch_version   = 0
milestone       = 'alpha'
build_type      = get_option('buildtype')
git_commit_hash = 'unknown'
git_branch      = 'unknown'
suffix          = ''

if compiler.get_id() == 'msvc' or compiler.get_id() == 'clang-cl'
    c_arguments += ['/std:clatest', '/DNOMINMAX', '/DUNICODE', '/D_ENABLE_EXTENDED_ALIGNED_STORAGE', '/nologo']
    cpp_arguments += ['/std:c++latest', '/DNOMINMAX', '/DUNICODE', '/D_ENABLE_EXTENDED_ALIGNED_STORAGE', '/nologo', '/Zc:__cplusplus']
    lib_extension = '.lib'

    if build_type == 'debug'
        c_arguments += ['/DSTORMKIT_BUILD_DEBUG']
        cpp_arguments += ['/DSTORMKIT_BUILD_DEBUG']
    endif

    if compiler.get_id() == 'clang-cl'
        c_arguments += ['-fcolor-diagnostics', '-fdiagnostics-color', '-Wno-missing-braces', '-Wno-unknown-pragmas', '-Wno-unused-command-line-argument', '-fansi-escape-codes']
        cpp_arguments += ['-fcolor-diagnostics', '-fdiagnostics-color', '-Wno-missing-braces', '-Wno-unknown-pragmas', '-Wno-unused-command-line-argument', '-fansi-escape-codes']
    endif

    if lib_type == 'static'
        c_arguments += ['/DSTORMKIT_STATIC']
        cpp_arguments += ['/DSTORMKIT_STATIC']
    endif
else
    c_arguments += ['-std=c2x', '-Wno-missing-braces', '-Wno-unknown-pragmas']
    cpp_arguments += ['-std=c++2a', '-Wno-missing-braces', '-Wno-unknown-pragmas']

    if is_windows
        lib_extension = '.a'
    elif is_macos
        lib_extension = '.dylib'
    else
        lib_extension = '.so'
    endif

    if build_type == 'debug'
        c_arguments += ['-DSTORMKIT_BUILD_DEBUG']
        cpp_arguments += ['-DSTORMKIT_BUILD_DEBUG']
    endif

    if lib_type == 'static'
        c_arguments += ['-DSTORMKIT_STATIC']
        cpp_arguments += ['-DSTORMKIT_STATIC']
        lib_extension = '.a'
    endif
endif

add_project_arguments(c_arguments, language: ['c', 'objc'])
add_project_arguments(cpp_arguments, language: ['cpp', 'objcpp'])
add_project_arguments(['-DSTORMKIT_BUILD'], language: ['c', 'objc', 'cpp', 'objcpp'])

enable_doc            = get_option('enable_doc')
enable_entities       = get_option('enable_entities')
enable_log            = get_option('enable_log')
enable_module         = get_option('enable_module')
enable_window         = get_option('enable_window')
enable_window_xcb     = get_option('enable_window_xcb')
enable_window_wayland = get_option('enable_window_wayland')
enable_image          = get_option('enable_image')
enable_render         = get_option('enable_render')
enable_engine         = get_option('enable_engine')

copy_to_bindir = get_option('copy_to_bindir')


if build_type == 'debug' or build_type == 'debugoptimized'
    suffix += '-debug'
endif

git_command = find_program('git', required: false)
doxygen = find_program('doxygen', required: false)
if not doxygen.found()
  message('Doxygen not found. Disabling doc generation')
  enable_doc = false
endif

sphinx = find_program('sphinx-build', required: false)
if not sphinx.found()
  message('Sphinx not found. Disabling doc generation')
  enable_doc = false
endif

if not git_command.found()
    message('Failed to extract git commit hash: \ngit not found')
else
    git_commit_hash_run = run_command('git', ['log', '-1', '--pretty=%h'])
    git_branch_run = run_command('git', ['rev-parse', '--abbrev-ref', 'HEAD'])

    if git_commit_hash_run.returncode() == 0
        git_commit_hash = git_commit_hash_run.stdout().strip()
    else
        message('Failed to extract git commit hash: \n' + git_commit_hash_run.stderr())
    endif

    if git_branch_run.returncode() == 0
        git_branch = git_branch_run.stdout().strip()
    else
        message('Failed to extract git branch: \n' + git_branch_run.stderr())
    endif
endif

if copy_to_bindir
    copy_file = find_program('utils/copy_to_bindir.py')
endif

summary({
    'Enable doc': enable_doc,
    'Enable examples': get_option('enable_examples'),
    'examples_data_dir': examples_data_dir,
    'library_type': get_option('library_type'),
    'build_type': build_type,
}, section: 'Configuration')
summary({
     'enable_log': enable_log,
     'enable_entities': enable_entities,
     'enable_module': enable_module,
     'enable_window': enable_window,
     'enable_window_xcb': enable_window_xcb,
     'enable_window_wayland': enable_window_wayland,
     'enable_image': enable_image,
     'enable_render': enable_render,
     'enable_engine': enable_engine,
}, section: 'Modules')
summary({
    'bindir': get_option('bindir'),
    'libdir': get_option('libdir'),
    'datadir': get_option('datadir'),
}, section: 'Directories')

if enable_render
    #assert(enable_image and enable_window and enable_log)
endif 

if enable_engine
    #assert(enable_render)
endif 

subdir('core/')
stormkit_deps = [core_dep]

if enable_entities
    subdir('entities/')
    stormkit_deps += [entities_dep]
endif

if enable_log
    subdir('log/')
    stormkit_deps += [log_dep]
endif

if enable_module
    subdir('module/')
    stormkit_deps += [module_dep]
endif

if enable_window
    subdir('main/')
    subdir('window/')
    stormkit_deps += [main_dep, window_dep]
endif

if enable_image
    subdir('image/')
    stormkit_deps += [image_dep]
endif

if enable_render
    subdir('render/')
    stormkit_deps += [render_dep]
endif

if enable_engine
    subdir('engine/')
    stormkit_deps += [engine_dep]
endif

stormkit_dep = declare_dependency(
    dependencies: stormkit_deps
)

cmake = import('cmake')
cmake.write_basic_package_version_file(
    name: 'StormKit',
    version: meson.project_version(),
    compatibility: 'SameMajorVersion'
)

conf = configuration_data()
cmake.configure_package_config_file(
    name: 'StormKit',
    input: 'StormKitConfig.cmake.in',
    configuration: conf
)

configure_file(input : 'StormKitStaticTargets.cmake.in',
               output : 'StormKitStaticTargets.cmake',
               copy: true)
configure_file(input : 'StormKitStaticTargets-debug.cmake.in',
               output : 'StormKitStaticTargets-debug.cmake',
               copy: true)
configure_file(input : 'StormKitStaticTargets-release.cmake.in',
               output : 'StormKitStaticTargets-release.cmake',
               copy: true)

configure_file(input : 'StormKitSharedTargets.cmake.in',
               output : 'StormKitSharedTargets.cmake',
               copy: true)
configure_file(input : 'StormKitSharedTargets-debug.cmake.in',
               output : 'StormKitSharedTargets-debug.cmake',
               copy: true)
configure_file(input : 'StormKitSharedTargets-release.cmake.in',
               output : 'StormKitSharedTargets-release.cmake',
               copy: true)

if lib_type == 'static'
    install_data(files(meson.current_build_dir() / 'StormKitStaticTargets.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    if get_option('buildtype') == 'debug'
        install_data(files(meson.current_build_dir() / 'StormKitStaticTargets-debug.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    else
        install_data(files(meson.current_build_dir() / 'StormKitStaticTargets-release.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    endif
else
    install_data(files(meson.current_build_dir() / 'StormKitSharedTargets.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    if get_option('buildtype') == 'debug'
        install_data(files(meson.current_build_dir() / 'StormKitSharedTargets-debug.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    else
        install_data(files(meson.current_build_dir() / 'StormKitSharedTargets-release.cmake'), install_dir: get_option('libdir') / 'cmake/StormKit')
    endif
endif

