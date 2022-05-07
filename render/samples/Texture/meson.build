dependencies = [core_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process('shaders/vertex.vert.glsl', 'shaders/fragment.frag.glsl')

directory = 'stormkit/Texture/'

arguments = []
if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    arguments += ['/DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"']
else
    arguments += ['-DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"']
endif

arguments += examples_arguments

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

texture = executable(
    'stormkit_Texture',
    [sources, shader_files],
    dependencies: dependencies,
    cpp_args: arguments,
    build_rpath: './',
    install: true
)
destination_path = join_paths(meson.project_build_root(), 'output/bin/')

if copy_to_bindir
    texture_copy_target = custom_target(
        'texture_copy_target',
        input: texture,
        output: 'texture-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
    texture_data_copy_target = custom_target(
        'texture_data_copy_target',
        input: files('textures/texture.png'),
        output: 'texture-fake2',
        command: [copy_file, '@INPUT@', join_paths(destination_path, 'textures')],
        install: false,
        build_by_default: true
    )
endif



install_subdir('textures/', install_dir: get_option('datadir') / directory)
