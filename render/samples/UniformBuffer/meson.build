dependencies = [core_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process('shaders/vertex.vert.glsl', 'shaders/fragment.frag.glsl')

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

uniform_buffer = executable(
    'stormkit_UniformBuffer',
    [sources, shader_files],
    dependencies: dependencies,
    cpp_args: examples_arguments,
    build_rpath: './',
    install: true
)
destination_path = join_paths(meson.project_build_root(), 'output/bin/')

if copy_to_bindir
    uniform_buffer_copy_target = custom_target(
        'uniform_buffer_copy_target',
        input: uniform_buffer,
        output: 'uniform-buffer-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif
