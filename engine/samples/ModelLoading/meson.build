dependencies = [core_dep, engine_dep, render_dep, window_dep, main_dep]

directory = 'stormkit/ModelLoading/'

arguments = []
if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    arguments += ['/DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '/DSTORMKIT_ENABLE_VALIDATION_LAYERS']
else
    arguments += ['-DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '-DSTORMKIT_ENABLE_VALIDATION_LAYERS']
endif

arguments += examples_arguments

#ide:editable-filelist
headers = files([
    'src/App.hpp',
    'src/MainState.hpp',
    'src/Constants.hpp',
    'src/Log.hpp',
])

sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/MainState.cpp',
])

model_loading = executable(
    'stormkit_ModelLoading',
    sources,
    dependencies: dependencies,
    cpp_args: arguments,
    extra_files: headers,
    build_rpath: './',
    install: true
)

destination_path = join_paths(meson.project_build_root(), 'output/bin/')

if copy_to_bindir
    model_loading_copy_target = custom_target(
        'model_loading_copy_target',
        input: model_loading,
        output: 'modelloading-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif

install_subdir('models/', install_dir: get_option('datadir') / directory)
install_subdir('textures/', install_dir: get_option('datadir') / directory)
