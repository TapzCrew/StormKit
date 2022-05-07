dependencies = [core_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process('shaders/vertex.vert.glsl', 'shaders/fragment.frag.glsl')

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

triangle = executable(
    'stormkit_Triangle',
    [sources, shader_files],
    dependencies: dependencies,
    cpp_args: examples_arguments,
    build_rpath: './',
    install: true
)

destination_path = join_paths(meson.project_build_root(), 'output/bin/')

if copy_to_bindir
    triangle_copy_target = custom_target(
        'triangle_copy_target',
        input: triangle,
        output: 'triangle-fake',
        command: [copy_file, '@INPUT@', destination_path],
        install: false,
        build_by_default: true
    )
endif
